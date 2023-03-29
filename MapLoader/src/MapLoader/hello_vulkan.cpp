/*
 * Copyright (c) 2014-2021, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION
 * SPDX-License-Identifier: Apache-2.0
 */


#include <sstream>
#include <filesystem>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "obj_loader.h"
#include <tinygltf/stb_image.h>
#include <tinygltf/stb_image_write.h>

#include "hello_vulkan.h"
#include "nvh/alignment.hpp"
#include "nvh/cameramanipulator.hpp"
#include "nvh/fileoperations.hpp"
#include "nvvk/commands_vk.hpp"
#include "nvvk/descriptorsets_vk.hpp"
#include "nvvk/images_vk.hpp"
#include "nvvk/pipeline_vk.hpp"
#include "nvvk/renderpasses_vk.hpp"
#include "nvvk/shaders_vk.hpp"
#include "nvvk/buffers_vk.hpp"
#include "nvvk/memorymanagement_vk.hpp"
#include <ArchiveParser/ParserUtils.h>
#include <ArchiveParser/MetadataMapper.h>
#include <Engine/Math/Color4.h>
#include <Engine/Math/Color3I.h>

extern std::vector<std::string> defaultSearchPaths;

//--------------------------------------------------------------------------------------------------
// Keep the handle on the device
// Initialize the tool to do all our allocations: buffers, images
//
void HelloVulkan::setup(const VkInstance& instance, const VkDevice& device, const VkPhysicalDevice& physicalDevice, uint32_t queueFamily)
{
	AppBaseVk::setup(instance, device, physicalDevice, queueFamily);
	VulkanContext->Allocator.init(instance, device, physicalDevice);
	VulkanContext->Debug.setup(VulkanContext->Device);
	ShaderLibrary = std::make_unique<Graphics::ShaderLibrary>(VulkanContext);
	m_offscreenDepthFormat = nvvk::findDepthFormat(physicalDevice);
}

//--------------------------------------------------------------------------------------------------
// Called at each frame to update the camera matrix
//
void HelloVulkan::updateUniformBuffer(const VkCommandBuffer& cmdBuf)
{
	// Prepare new UBO contents on host.
	const float    aspectRatio = m_size.width / static_cast<float>(m_size.height);
	const auto&    view        = CameraManip.getMatrix();
	const auto& proj = mat4::NewProjection(CameraManip.getFov()*nv_to_rad, aspectRatio, 0.1f, 1000.0f)*mat4::NewScale(1,-1,1);
	// proj[1][1] *= -1;  // Inverting Y for Vulkan (not needed with perspectiveVK).

	hostUBO.viewProj    = proj * view;
	hostUBO.viewInverse = view.Inverted();
	hostUBO.projInverse = proj.Inverted();
	hostUBO.lightCount = (int)lights.size();

	if (m_mouseIO.memHandle != nullptr)
	{
		mouseIOPtr = reinterpret_cast<MouseRayOut*>(VulkanContext->Allocator.map(m_mouseIO));
		if (mouseIOPtr != nullptr)
			mouseIO = *mouseIOPtr;
		VulkanContext->Allocator.unmap(m_mouseIO);
	}

	// UBO on the device, and what stages access it.
	VkBuffer deviceUBO      = m_bGlobals.buffer;
	auto     uboUsageStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

	// Ensure that the modified UBO is not visible to previous frames.
	VkBufferMemoryBarrier beforeBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
	beforeBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	beforeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	beforeBarrier.buffer        = deviceUBO;
	beforeBarrier.offset        = 0;
	beforeBarrier.size          = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmdBuf, uboUsageStages, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
						 nullptr, 1, &beforeBarrier, 0, nullptr);


	// Schedule the host-to-device upload. (hostUBO is copied into the cmd
	// buffer so it is okay to deallocate when the function returns).
	vkCmdUpdateBuffer(cmdBuf, m_bGlobals.buffer, 0, sizeof(GlobalUniforms), &hostUBO);

	// Making sure the updated UBO will be visible.
	VkBufferMemoryBarrier afterBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
	afterBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	afterBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	afterBarrier.buffer        = deviceUBO;
	afterBarrier.offset        = 0;
	afterBarrier.size          = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, uboUsageStages, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
						 nullptr, 1, &afterBarrier, 0, nullptr);
}

//--------------------------------------------------------------------------------------------------
// Describing the layout pushed when rendering
//
void HelloVulkan::createDescriptorSetLayout()
{
	std::vector<Engine::Graphics::VertexAttributeFormat> attributes;

	attributes.push_back(Engine::Graphics::VertexAttributeFormat{ Engine::Graphics::AttributeDataTypeEnum::Float32, 3, "position", 0 });
	attributes.push_back(Engine::Graphics::VertexAttributeFormat{ Engine::Graphics::AttributeDataTypeEnum::Float32, 3, "normal", 0 });
	attributes.push_back(Engine::Graphics::VertexAttributeFormat{ Engine::Graphics::AttributeDataTypeEnum::Float32, 4, "COLOR", 0 });
	attributes.push_back(Engine::Graphics::VertexAttributeFormat{ Engine::Graphics::AttributeDataTypeEnum::Float32, 2, "textureCoords", 0 });
	attributes.push_back(Engine::Graphics::VertexAttributeFormat{ Engine::Graphics::AttributeDataTypeEnum::Float32, 3, "binormal", 0 });
	attributes.push_back(Engine::Graphics::VertexAttributeFormat{ Engine::Graphics::AttributeDataTypeEnum::Float32, 3, "tangent", 0 });

	ShaderVertexFormat = Engine::Graphics::MeshFormat::GetFormat(attributes);

	DescriptorSetLibrary = std::make_shared<Graphics::DescriptorSetLibrary>(VulkanContext);
	RasterPipeline = std::make_unique<Graphics::ShaderPipeline>(VulkanContext, DescriptorSetLibrary);
	RasterPipeline->BindDescriptorSet("common", 1);
	RasterPipeline->SetVertexFormat(ShaderVertexFormat);

	{
		Graphics::Shader* shaders[] = {
			ShaderLibrary->FetchShader("vert_shader.vert", VK_SHADER_STAGE_VERTEX_BIT),
			ShaderLibrary->FetchShader("frag_shader.frag", VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		for (auto& shader : shaders)
		{
			RasterPipeline->AddStage(shader);
		}
	}

	RasterPipeline->LoadDescriptors();

	Shaders = {
		ShaderLibrary->FetchShader("raytrace.rgen", VK_SHADER_STAGE_RAYGEN_BIT_KHR),
		ShaderLibrary->FetchShader("raytrace.rmiss", VK_SHADER_STAGE_MISS_BIT_KHR),
		ShaderLibrary->FetchShader("raytraceShadow.rmiss", VK_SHADER_STAGE_MISS_BIT_KHR),
		ShaderLibrary->FetchShader("raytrace.rchit", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
		ShaderLibrary->FetchShader("raytrace.rahit", VK_SHADER_STAGE_ANY_HIT_BIT_KHR),
		ShaderLibrary->FetchShader("raytraceShadow.rchit", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
		ShaderLibrary->FetchShader("raytraceShadow.rahit", VK_SHADER_STAGE_ANY_HIT_BIT_KHR),
	};

	RTPipeline = std::make_unique<Graphics::ShaderPipeline>(VulkanContext, DescriptorSetLibrary);
	RTPipeline->BindDescriptorSet("common", 1);

	for (size_t i = 0; i < Shaders.size(); ++i)
	{
		RTPipeline->AddStage(Shaders[i]);
	}

	RTPipeline->LoadDescriptors();
	RTPipeline->AddRayGenGroup(0);
	RTPipeline->AddMissGroup(1);
	RTPipeline->AddMissGroup(2);
	RTPipeline->AddHitGroup({ 3, 4 });
	RTPipeline->AddHitGroup({ 5, 6 });

	PostPipeline = std::make_unique<Graphics::ShaderPipeline>(VulkanContext, DescriptorSetLibrary);

	{
		Graphics::Shader* shaders[] = {
			ShaderLibrary->FetchShader("passthrough.vert", VK_SHADER_STAGE_VERTEX_BIT),
			ShaderLibrary->FetchShader("post.frag", VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		for (auto& shader : shaders)
		{
			PostPipeline->AddStage(shader);
		}
	}

	PostPipeline->LoadDescriptors();

	uint32_t textureCount = (uint32_t)AssetLibrary->GetTextures().GetAssets().size();

	auto& descriptor = DescriptorSetLibrary->FetchDescriptorSet("common")->FetchBinding(eTextures).Binding;

	descriptor.descriptorCount = textureCount;

	DescriptorSetLibrary->CreateLayouts();
	DescriptorSetLibrary->CreateDescriptorPool();
	DescriptorSetLibrary->CreateDescriptorSets();

	VkPhysicalDeviceProperties2 prop2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	prop2.pNext = &VulkanContext->RTDeviceProperties;
	vkGetPhysicalDeviceProperties2(m_physicalDevice, &prop2);
}

//--------------------------------------------------------------------------------------------------
// Setting up the buffers in the descriptor set
//
void HelloVulkan::updateDescriptorSet()
{
	AssetLibrary->GetTextures().FlushLoadingQueue();
	std::vector<VkWriteDescriptorSet> writes;

	auto m_descSet = DescriptorSetLibrary->FetchDescriptorSet("common");

	// Camera matrices and scene description
	VkDescriptorBufferInfo dbiUnif{m_bGlobals.buffer, 0, VK_WHOLE_SIZE};
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eGlobals, &dbiUnif));

	VkDescriptorBufferInfo dbiSceneDesc{m_bObjDesc.buffer, 0, VK_WHOLE_SIZE};
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eObjDescs, &dbiSceneDesc));

	// All texture samplers
	std::vector<VkDescriptorImageInfo> diit;
	for(auto& texture : AssetLibrary->GetTextures().GetAssets())
	{
	diit.emplace_back(texture.Data.descriptor);
	}
	writes.emplace_back(m_descSet->MakeWriteArray(SceneBindings::eTextures, diit.data()));

	VkDescriptorBufferInfo dbiLightDesc{ m_lightDesc.buffer, 0, VK_WHOLE_SIZE };
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eLights, &dbiLightDesc));

	VkDescriptorBufferInfo dbiTextureTransformDesc{ m_textureTransformDesc.buffer, 0, VK_WHOLE_SIZE };
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eTextureTransforms, &dbiTextureTransformDesc));

	VkDescriptorBufferInfo dbiMouseIODesc{ m_mouseIO.buffer, 0, VK_WHOLE_SIZE };
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eMouseIO, &dbiMouseIODesc));

	VkDescriptorBufferInfo dbiInstDesc{ m_InstDesc.buffer, 0, VK_WHOLE_SIZE };
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eInstDescs, &dbiInstDesc));

	VkDescriptorBufferInfo dbiTextureOverrideDesc{ m_textureOverride.buffer, 0, VK_WHOLE_SIZE };
	writes.emplace_back(m_descSet->MakeWrite(SceneBindings::eTextureOverrides, &dbiTextureOverrideDesc));

	if (m_mouseIO.memHandle != nullptr)
	{
		//mouseIOPtr = reinterpret_cast<MouseRayOut*>(VulkanContext->Allocator.map(m_mouseIO));
		//if (mouseIOPtr != nullptr)
		//	mouseIO = *mouseIOPtr;
		//VulkanContext->Allocator.unmap(m_mouseIO);
	}

	// Writing the information
	vkUpdateDescriptorSets(VulkanContext->Device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}


//--------------------------------------------------------------------------------------------------
// Creating the pipeline layout
//
void HelloVulkan::createGraphicsPipeline()
{
	RasterPipeline->SetRenderPass(RasterRenderPass);
	RasterPipeline->SetAttachmentsAlphaBlend();
	RasterPipeline->CreatePipelineLayout();
	RasterPipeline->CreateRasterPipeline();
	//
	//// Creating the Pipeline
	//std::vector<std::string>                paths = defaultSearchPaths;
	//nvvk::GraphicsPipelineGeneratorCombined gpb(VulkanContext->Device, RasterPipeline->GetPipelineLayout(), m_offscreenRenderPass);
	//gpb.depthStencilState.depthTestEnable = true;
	//gpb.addShader(nvh::loadFile("spv/vert_shader.vert.spv", true, paths, true), VK_SHADER_STAGE_VERTEX_BIT);
	//gpb.addShader(nvh::loadFile("spv/frag_shader.frag.spv", true, paths, true), VK_SHADER_STAGE_FRAGMENT_BIT);
	//gpb.addBindingDescription({0, sizeof(VertexObj)});
	//gpb.addAttributeDescriptions({
	//	{0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(VertexObj, pos))},
	//	{1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(VertexObj, nrm))},
	//	{2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(VertexObj, color))},
	//	{3, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(VertexObj, texCoord))},
	//});
	//
	//m_graphicsPipeline = gpb.createPipeline();
	//VulkanContext->Debug.setObjectName(m_graphicsPipeline, "Graphics");
}

void HelloVulkan::loadModelInstance(uint32_t index, mat4 transform)
{
	ObjInstance instance;
	instance.transform = transform;
	instance.objIndex = index;
	m_instances.push_back(instance);
}

//--------------------------------------------------------------------------------------------------
// Creating the uniform buffer holding the camera matrices
// - Buffer is host visible
//
void HelloVulkan::createUniformBuffer()
{
	m_bGlobals = VulkanContext->Allocator.createBuffer(sizeof(GlobalUniforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
									VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VulkanContext->Debug.setObjectName(m_bGlobals.buffer, "Globals");
}

namespace nvvk
{
	class DMAMemoryHandle;
}

//--------------------------------------------------------------------------------------------------
// Create a storage buffer containing the description of the scene elements
// - Which geometry is used by which instance
// - Transformation
// - Offset for texture
//
void HelloVulkan::createObjDescriptionBuffer()
{
	nvvk::CommandPool cmdGen(VulkanContext->Device, VulkanContext->GraphicsQueueIndex);

	auto cmdBuf = cmdGen.createCommandBuffer();
	m_bObjDesc  = VulkanContext->Allocator.createBuffer(cmdBuf, AssetLibrary->GetModels().GetGpuData(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	cmdGen.submitAndWait(cmdBuf);
	VulkanContext->Allocator.finalizeAndReleaseStaging();
	VulkanContext->Debug.setObjectName(m_bObjDesc.buffer, "ObjDescs");

	if (lights.size() == 0)
		lights.push_back(LightDesc());

	auto cmdBuf2 = cmdGen.createCommandBuffer();
	m_lightDesc = VulkanContext->Allocator.createBuffer(cmdBuf2, lights, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	cmdGen.submitAndWait(cmdBuf2);
	VulkanContext->Allocator.finalizeAndReleaseStaging();
	VulkanContext->Debug.setObjectName(m_lightDesc.buffer, "LightDescs");

	const auto& libraryTextureTransforms = AssetLibrary->GetModels().GetTextureTransforms();
	static const std::vector<TextureTransform> defaultTextureTransforms{ {} };
	const auto& textureTransforms = libraryTextureTransforms.size() != 0 ? libraryTextureTransforms : defaultTextureTransforms;

	auto cmdBuf3 = cmdGen.createCommandBuffer();
		m_textureTransformDesc = VulkanContext->Allocator.createBuffer(cmdBuf3, textureTransforms, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	cmdGen.submitAndWait(cmdBuf3);
	VulkanContext->Allocator.finalizeAndReleaseStaging();
	VulkanContext->Debug.setObjectName(m_textureTransformDesc.buffer, "TextureTransformDescs");

	auto cmdBuf4 = cmdGen.createCommandBuffer();
	m_InstDesc = VulkanContext->Allocator.createBuffer(cmdBuf4, instanceDescriptions, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	cmdGen.submitAndWait(cmdBuf4);
	VulkanContext->Allocator.finalizeAndReleaseStaging();
	VulkanContext->Debug.setObjectName(m_InstDesc.buffer, "InstanceDescs");

	auto cmdBuf5 = cmdGen.createCommandBuffer();

	std::vector<MouseRayOut> out = { { vec3(), -1 } };

	//m_mouseIO = VulkanContext->Allocator.createBuffer(sizeof(mouseIO), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//cmdGen.submitAndWait(cmdBuf4);
	//VulkanContext->Allocator.getDMA()->map(m_mouseIO.memHandle, 0, sizeof(mouseIO), &mouseIO);
	//VulkanContext->Allocator.finalizeAndReleaseStaging();
	m_mouseIO = VulkanContext->Allocator.createBuffer(cmdBuf5, out, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
	cmdGen.submitAndWait(cmdBuf5);
	VulkanContext->Allocator.finalizeAndReleaseStaging();

	VulkanContext->Allocator.finalizeAndReleaseStaging();
	VulkanContext->Debug.setObjectName(m_mouseIO.buffer, "MouseIODescs");

	const auto& libraryMaterialTextures = AssetLibrary->GetModels().GetMaterialTextures();
	static const std::vector<MaterialTextures> defaultMaterialTextures{ {} };
	const auto& materialTextures = libraryMaterialTextures.size() == 0 ? defaultMaterialTextures : libraryMaterialTextures;
	
	auto cmdBuf6 = cmdGen.createCommandBuffer();
	m_textureOverride = VulkanContext->Allocator.createBuffer(cmdBuf6, materialTextures, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	cmdGen.submitAndWait(cmdBuf6);
	VulkanContext->Allocator.finalizeAndReleaseStaging();
	VulkanContext->Debug.setObjectName(m_textureOverride.buffer, "TextureOverrides");
}

bool HelloVulkan::startScreenshot()
{
	if (takingScreenshot) return false;

	takingScreenshot = true;

	m_offscreenSize = { m_batchSize.width * m_size.width, m_batchSize.height * m_size.height };

	auto         imgSize = VkExtent2D{ (uint32_t)m_size.width, (uint32_t)m_size.height };

	auto         imageCreateInfo = nvvk::makeImage2DCreateInfo(imgSize, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_TRANSFER_DST_BIT, true);
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;

	screenshotImage = VulkanContext->Allocator.createImage(imageCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkExtent2D size = m_size;

	if (combineScreenshot)
	{
		size.width *= m_batchSize.width;
		size.height *= m_batchSize.height;
	}

	VkDeviceSize bufferSize = static_cast<uint64_t>(size.width) * size.height * sizeof(uint8_t) * 3;
	screenshotBuffer.resize(bufferSize);

	std::cout << "starting screenshot" << std::endl;

	return true;
}

void HelloVulkan::screenshot()
{
	VkExtent2D nextBatch = m_batch;

	nextBatch.width++;

	if (nextBatch.width >= m_batchSize.width)
	{
		nextBatch.width = 0;
		nextBatch.height++;
	}

	bool flush = !takingScreenshot || nextBatch.height >= m_batchSize.height;

	nvvk::CommandPool  cmdBufGet(VulkanContext->Device, VulkanContext->GraphicsQueueIndex);
	VkCommandBuffer    copyCmd = cmdBufGet.createCommandBuffer();

	bool justStarted = startScreenshot();

	nvvk::cmdBarrierImageLayout(copyCmd, m_offscreenColor.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	nvvk::cmdBarrierImageLayout(copyCmd, screenshotImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Otherwise use image copy (requires us to manually flip components)
	VkImageCopy imageCopyRegion{};
	imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopyRegion.srcSubresource.layerCount = 1;
	imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopyRegion.dstSubresource.layerCount = 1;
	imageCopyRegion.extent.width = m_size.width;
	imageCopyRegion.extent.height = m_size.height;
	imageCopyRegion.extent.depth = 1;

	// Issue the copy command
	vkCmdCopyImage(
		copyCmd,
		m_offscreenColor.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		screenshotImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageCopyRegion);

	nvvk::cmdBarrierImageLayout(copyCmd, m_offscreenColor.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	nvvk::cmdBarrierImageLayout(copyCmd, screenshotImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	cmdBufGet.submitAndWait(copyCmd);

	VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
	VkSubresourceLayout subResourceLayout{ 0 };
	vkGetImageSubresourceLayout(VulkanContext->Device, screenshotImage.image, &subResource, &subResourceLayout);

	const char* data = reinterpret_cast<const char*>(VulkanContext->Allocator.map(screenshotImage));

	data += subResourceLayout.offset;

	for (uint32_t y = 0; y < m_size.height; y++)
	{
		const Color4* row = reinterpret_cast<const Color4*>(data);
		uint32_t yOffset = y;

		if (combineScreenshot)
			yOffset += m_batch.height * m_size.height;

		Color3I* outRow = reinterpret_cast<Color3I*>(screenshotBuffer.data()) + (yOffset) *m_offscreenSize.width;

		for (uint32_t x = 0; x < m_size.width; x++)
		{
			if (combineScreenshot)
				outRow[x + m_batch.width * m_size.width] = row[x];
			else
				outRow[x] = row[x];
		}
		data += subResourceLayout.rowPitch;
	}

	if (takingScreenshot && !combineScreenshot)
	{
		std::stringstream outPath;
		outPath << "./screenshots/screenshot_";
		outPath << m_batch.width << "_" << m_batch.height << ".png";

		stbi_write_png(outPath.str().c_str(), (int)m_size.width, (int)m_size.height, 3, screenshotBuffer.data(), (int)(m_size.width * sizeof(Color3I)));
	}
	else if (flush)
	{
		stbi_write_png("./screenshot.png", (int)m_offscreenSize.width, (int)m_offscreenSize.height, 3, screenshotBuffer.data(), (int)(m_offscreenSize.width * sizeof(Color3I)));
		std::cout << "saved screenshot" << std::endl;
	}

	if (flush)
	{
		screenshotBuffer.clear();
		screenshotBuffer.shrink_to_fit();

		VulkanContext->Allocator.unmap(screenshotImage);
		VulkanContext->Allocator.destroy(screenshotImage);

		takingScreenshot = false;
		std::cout << "finished screenshot" << std::endl;
		m_batch = { 0, 0 };
	}
	else
	{
		std::cout << "wrote batch " << m_batch.width << ", " << m_batch.height << std::endl;

		m_batch = nextBatch;
	}
}

//--------------------------------------------------------------------------------------------------
// Destroying all allocations
//
void HelloVulkan::destroyResources()
{
	DescriptorSetLibrary->ReleaseResources();

	RasterPipeline->ReleaseResources();
	DeviceRenderPass->ReleaseResources();
	ShaderLibrary->ReleaseModules();

	for (size_t i = 0; i < 3; ++i)
		DeviceBuffers[i]->ReleaseResources();

	VulkanContext->Allocator.destroy(m_bGlobals);
	VulkanContext->Allocator.destroy(m_bObjDesc);
	VulkanContext->Allocator.destroy(m_lightDesc);
	VulkanContext->Allocator.destroy(m_textureTransformDesc);
	VulkanContext->Allocator.destroy(m_mouseIO);
	VulkanContext->Allocator.destroy(m_InstDesc);
	VulkanContext->Allocator.destroy(m_textureOverride);

	AssetLibrary->GetModels().FreeResources();
	AssetLibrary->GetTextures().FreeResources();

	//#Post
	VulkanContext->Allocator.destroy(m_offscreenColor);
	VulkanContext->Allocator.destroy(m_offscreenDepth);
	PostPipeline->ReleaseResources();
	
	RasterRenderPass->ReleaseResources();
	OffscreenBuffer->ReleaseResources();

	// #VKRay
	Scene->FreeResources();
	RTPipeline->ReleaseResources();

	VulkanContext->Allocator.deinit();
}

//--------------------------------------------------------------------------------------------------
// Drawing the scene in raster mode
//
void HelloVulkan::rasterize(const VkCommandBuffer& cmdBuf)
{
	VkDeviceSize offset{0};

	VulkanContext->Debug.beginLabel(cmdBuf, "Rasterize");

	// Dynamic Viewport
	setViewport(cmdBuf);

	auto& m_descSet = DescriptorSetLibrary->FetchDescriptorSet("common")->DescriptorSet;

	// Drawing all triangles
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, RasterPipeline->GetPipeline());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, RasterPipeline->GetPipelineLayout(), 0, 1, &m_descSet, 0, nullptr);


	for(const HelloVulkan::ObjInstance& inst : m_instances)
	{
	auto& model            = AssetLibrary->GetModels().GetMeshDescriptions()[inst.objIndex];
	m_pcRaster.objIndex    = inst.objIndex;  // Telling which object is drawn
	m_pcRaster.modelMatrix = inst.transform;

	vkCmdPushConstants(cmdBuf, RasterPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
						 sizeof(PushConstantRaster), &m_pcRaster);
	vkCmdBindVertexBuffers(cmdBuf, 0, 1, &model.VertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(cmdBuf, model.IndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmdBuf, model.IndexCount, 1, 0, 0, 0);
	}
	VulkanContext->Debug.endLabel(cmdBuf);
}

//--------------------------------------------------------------------------------------------------
// Handling resize of the window
//
void HelloVulkan::onResize(int /*w*/, int /*h*/)
{
	createOffscreenRender();
	updatePostDescriptorSet();
	updateRtDescriptorSet();
}


//////////////////////////////////////////////////////////////////////////
// Post-processing
//////////////////////////////////////////////////////////////////////////
void HelloVulkan::createFrameBuffers()
{
	for (size_t i = 0; i < 3; ++i)
	{
		DeviceBuffers[i] = std::make_unique<Graphics::FrameBuffer>(DeviceRenderPass);
		DeviceBuffers[i]->SetResolution(m_size.width, m_size.height, 1);

		auto& attachments = DeviceBuffers[i]->GetAttachments();
		attachments[0] = m_swapChain.getImageView((uint32_t)i);
		attachments[1] = m_depthView;

		DeviceBuffers[i]->Create();

#ifdef _DEBUG
		std::string                   name = std::string("AppBase") + std::to_string(i);
		VkDebugUtilsObjectNameInfoEXT nameInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		nameInfo.objectHandle = (uint64_t)DeviceBuffers[i]->GetFrameBuffer();
		nameInfo.objectType = VK_OBJECT_TYPE_FRAMEBUFFER;
		nameInfo.pObjectName = name.c_str();
		vkSetDebugUtilsObjectNameEXT(VulkanContext->Device, &nameInfo);
#endif
	}
}

void HelloVulkan::createRenderPass()
{
	if (DeviceRenderPass.get() == nullptr)
	{
		DeviceRenderPass = std::make_shared<Graphics::RenderPass>(VulkanContext);
		DeviceRenderPass->Configure({ m_colorFormat }, m_depthFormat, true, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		DeviceRenderPass->AddSubpass();
		DeviceRenderPass->SetColorAttachments({ 0 }, { 0 });
		DeviceRenderPass->SetDepthAttachments({ 0 }, 1);
	}
	else
		DeviceRenderPass->ReleaseResources();

	DeviceRenderPass->Create();
}

//--------------------------------------------------------------------------------------------------
// Creating an offscreen frame buffer and the associated render pass
//
void HelloVulkan::createOffscreenRender()
{
	VulkanContext->Allocator.destroy(m_offscreenColor);
	VulkanContext->Allocator.destroy(m_offscreenDepth);

	// Creating the color image
	{
	auto colorCreateInfo = nvvk::makeImage2DCreateInfo(m_size, m_offscreenColorFormat,
												         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
												             | VK_IMAGE_USAGE_STORAGE_BIT);


	nvvk::Image           image  = VulkanContext->Allocator.createImage(colorCreateInfo);
	VkImageViewCreateInfo ivInfo = nvvk::makeImageViewCreateInfo(image.image, colorCreateInfo);
	VkSamplerCreateInfo   sampler{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
	m_offscreenColor                        = VulkanContext->Allocator.createTexture(image, ivInfo, sampler);
	m_offscreenColor.descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	}

	// Creating the depth buffer
	auto depthCreateInfo = nvvk::makeImage2DCreateInfo(m_size, m_offscreenDepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	{
	nvvk::Image image = VulkanContext->Allocator.createImage(depthCreateInfo);


	VkImageViewCreateInfo depthStencilView{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
	depthStencilView.viewType         = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilView.format           = m_offscreenDepthFormat;
	depthStencilView.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
	depthStencilView.image            = image.image;

	m_offscreenDepth = VulkanContext->Allocator.createTexture(image, depthStencilView);
	}

	// Setting the image layout for both color and depth
	{
	nvvk::CommandPool genCmdBuf(VulkanContext->Device, VulkanContext->GraphicsQueueIndex);
	auto              cmdBuf = genCmdBuf.createCommandBuffer();
	nvvk::cmdBarrierImageLayout(cmdBuf, m_offscreenColor.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	nvvk::cmdBarrierImageLayout(cmdBuf, m_offscreenDepth.image, VK_IMAGE_LAYOUT_UNDEFINED,
								VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

	genCmdBuf.submitAndWait(cmdBuf);
	}

	//// Creating a renderpass for the offscreen
	//if(!m_offscreenRenderPass)
	//{
	//m_offscreenRenderPass = nvvk::createRenderPass(VulkanContext->Device, {m_offscreenColorFormat}, m_offscreenDepthFormat, 1, true,
	//											     true, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL);
	//}
	if (RasterRenderPass.get() == nullptr)
	{
		RasterRenderPass = std::make_shared<Graphics::RenderPass>(VulkanContext);
		RasterRenderPass->Configure({ m_offscreenColorFormat }, m_offscreenDepthFormat, true, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL);
		RasterRenderPass->AddSubpass();
		RasterRenderPass->SetColorAttachments({ 0 }, { 0 });
		RasterRenderPass->SetDepthAttachments({ 0 }, 1);
		RasterRenderPass->Create();
	}

	OffscreenBuffer = std::make_unique<Graphics::FrameBuffer>(RasterRenderPass);
	OffscreenBuffer->SetResolution(m_size.width, m_size.height, 1);

	auto& attachments = OffscreenBuffer->GetAttachments();
	attachments[0] = m_offscreenColor.descriptor.imageView;
	attachments[1] = m_offscreenDepth.descriptor.imageView;

	OffscreenBuffer->Create();

	// Creating the frame buffer for offscreen
	//
	//vkDestroyFramebuffer(VulkanContext->Device, OffscreenBuffer->GetFrameBuffer(), nullptr);
	//VkFramebufferCreateInfo info{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
	//info.renderPass      = m_offscreenRenderPass;
	//info.attachmentCount = 2;
	//info.pAttachments    = attachments.data();
	//info.width           = m_size.width;
	//info.height          = m_size.height;
	//info.layers          = 1;
	//vkCreateFramebuffer(VulkanContext->Device, &info, nullptr, &m_offscreenFramebuffer);
}

//--------------------------------------------------------------------------------------------------
// The pipeline is how things are rendered, which shaders, type of primitives, depth test and more
//
void HelloVulkan::createPostPipeline()
{
	PostPipeline->SetRenderPass(DeviceRenderPass);
	PostPipeline->CreatePipelineLayout();
	PostPipeline->GetRasterizationStateCreateInfo().cullMode = VK_CULL_MODE_NONE;
	PostPipeline->CreateRasterPipeline();
	//PostPipeline->CreatePipelineLayout();
	//
	// Pipeline: completely generic, no vertices
	//nvvk::GraphicsPipelineGeneratorCombined pipelineGenerator(VulkanContext->Device, PostPipeline->GetPipelineLayout(), DeviceRenderPass->GetRenderPass());
	//pipelineGenerator.addShader(nvh::loadFile("spv/passthrough.vert.spv", true, defaultSearchPaths, true), VK_SHADER_STAGE_VERTEX_BIT);
	//pipelineGenerator.addShader(nvh::loadFile("spv/post.frag.spv", true, defaultSearchPaths, true), VK_SHADER_STAGE_FRAGMENT_BIT);
	//pipelineGenerator.rasterizationState.cullMode = VK_CULL_MODE_NONE;
	//m_postPipeline                                = pipelineGenerator.createPipeline();
	//VulkanContext->Debug.setObjectName(m_postPipeline, "post");
}

//--------------------------------------------------------------------------------------------------
// Update the output
//
void HelloVulkan::updatePostDescriptorSet()
{
	VkDescriptorBufferInfo dbiUnif{ m_bGlobals.buffer, 0, VK_WHOLE_SIZE };
	auto postDescSet = PostPipeline->GetDescriptorSets()[0];
	VkWriteDescriptorSet writeDescriptorSets[2] = {
		postDescSet->MakeWrite(0, &m_offscreenColor.descriptor),
		postDescSet->MakeWrite(1, &dbiUnif)
	};
	vkUpdateDescriptorSets(VulkanContext->Device, sizeof(writeDescriptorSets) / sizeof(writeDescriptorSets[0]), writeDescriptorSets, 0, nullptr);
}

//--------------------------------------------------------------------------------------------------
// Draw a full screen quad with the attached image
//
void HelloVulkan::drawPost(VkCommandBuffer cmdBuf)
{
	VulkanContext->Debug.beginLabel(cmdBuf, "Post");

	setViewport(cmdBuf);

	auto aspectRatio = static_cast<float>(m_size.width) / static_cast<float>(m_size.height);
	vkCmdPushConstants(cmdBuf, PostPipeline->GetPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &aspectRatio);
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, PostPipeline->GetPipeline());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, PostPipeline->GetPipelineLayout(), 0, 1, &PostPipeline->GetDescriptorSets()[0]->DescriptorSet, 0, nullptr);
	vkCmdDraw(cmdBuf, 3, 1, 0, 0);

	VulkanContext->Debug.endLabel(cmdBuf);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------
// Initialize Vulkan ray tracing
// #VKRay
void HelloVulkan::initRayTracing()
{
	// Requesting ray tracing properties
	//VkPhysicalDeviceProperties2 prop2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
	//prop2.pNext = &VulkanContext->RTDeviceProperties;
	//vkGetPhysicalDeviceProperties2(m_physicalDevice, &prop2);
}

//--------------------------------------------------------------------------------------------------
// Convert an OBJ model into the ray tracing geometry used to build the BLAS
//
auto HelloVulkan::objectToVkGeometryKHR(const MapLoader::MeshDescription& model)
{
	// BLAS builder requires raw device addresses.
	VkDeviceAddress vertexAddress = nvvk::getBufferDeviceAddress(VulkanContext->Device, model.VertexBuffer.buffer);
	VkDeviceAddress indexAddress  = nvvk::getBufferDeviceAddress(VulkanContext->Device, model.IndexBuffer.buffer);

	uint32_t maxPrimitiveCount = model.IndexCount / 3;

	// Describe buffer as array of VertexObj.
	VkAccelerationStructureGeometryTrianglesDataKHR triangles{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR};
	triangles.vertexFormat             = VK_FORMAT_R32G32B32_SFLOAT;  // vec3 vertex position data
	triangles.vertexData.deviceAddress = vertexAddress;
	triangles.vertexStride             = sizeof(VertexObj);
	// Describe index data (32-bit unsigned int)
	triangles.indexType               = VK_INDEX_TYPE_UINT32;
	triangles.indexData.deviceAddress = indexAddress;
	// Indicate identity transform by setting transformData to null device pointer.
	//triangles.transformData = {};
	triangles.maxVertex = model.VertexCount;

	// Identify the above data as containing opaque triangles.
	VkAccelerationStructureGeometryKHR asGeom{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR};
	asGeom.geometryType       = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags              = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;
	asGeom.geometry.triangles = triangles;

	// The entire array will be used to build the BLAS.
	VkAccelerationStructureBuildRangeInfoKHR offset{ 0 };
	offset.firstVertex     = 0;
	offset.primitiveCount  = maxPrimitiveCount;
	offset.primitiveOffset = 0;
	offset.transformOffset = 0;

	// Our blas is made from only one geometry, but could be made of many geometries
	nvvk::RaytracingBuilderKHR::BlasInput input;
	input.asGeometry.emplace_back(asGeom);
	input.asBuildOffsetInfo.emplace_back(offset);

	return input;
}

//--------------------------------------------------------------------------------------------------
//
//
void HelloVulkan::createBottomLevelAS()
{
	// BLAS - Storing each primitive in a geometry
	std::vector<nvvk::RaytracingBuilderKHR::BlasInput> allBlas;
	allBlas.reserve(AssetLibrary->GetModels().GetMeshDescriptions().size());
	for(const auto& obj : AssetLibrary->GetModels().GetMeshDescriptions())
	{
	auto blas = objectToVkGeometryKHR(obj);

	// We could add more geometry in each BLAS, but we add only one for now
	allBlas.emplace_back(blas);
	}
	Scene->GetRTBuilder().buildBlas(allBlas, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}

//--------------------------------------------------------------------------------------------------
// This descriptor set holds the Acceleration structure and the output image
//
void HelloVulkan::createRtDescriptorSet()
{
	auto& descriptorSets = RTPipeline->GetDescriptorSets();

	VkAccelerationStructureKHR                   tlas = Scene->GetRTBuilder().getAccelerationStructure();
	VkWriteDescriptorSetAccelerationStructureKHR descASInfo{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR};
	descASInfo.accelerationStructureCount = 1;
	descASInfo.pAccelerationStructures    = &tlas;
	VkDescriptorImageInfo imageInfo{{}, m_offscreenColor.descriptor.imageView, VK_IMAGE_LAYOUT_GENERAL};



	auto rtDescSet = RTPipeline->GetDescriptorSets()[0];

	std::vector<VkWriteDescriptorSet> writes;
	writes.emplace_back(rtDescSet->MakeWrite(RtxBindings::eTlas, &descASInfo));
	writes.emplace_back(rtDescSet->MakeWrite(RtxBindings::eOutImage, &imageInfo));
	vkUpdateDescriptorSets(VulkanContext->Device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

}


//--------------------------------------------------------------------------------------------------
// Writes the output image to the descriptor set
// - Required when changing resolution
//
void HelloVulkan::updateRtDescriptorSet()
{
	auto& descriptorSets = RTPipeline->GetDescriptorSets();
	// (1) Output buffer
	auto rtDescSet = RTPipeline->GetDescriptorSets()[0];
	VkDescriptorImageInfo imageInfo{{}, m_offscreenColor.descriptor.imageView, VK_IMAGE_LAYOUT_GENERAL};
	VkWriteDescriptorSet  wds = rtDescSet->MakeWrite(RtxBindings::eOutImage, &imageInfo);
	vkUpdateDescriptorSets(VulkanContext->Device, 1, &wds, 0, nullptr);
}


//--------------------------------------------------------------------------------------------------
// Pipeline for the ray tracer: all shaders, raygen, chit, miss
//
void HelloVulkan::createRtPipeline()
{
	RTPipeline->CreatePipelineLayout();
	RTPipeline->CreateRTPipeline();
}

//--------------------------------------------------------------------------------------------------
// Ray Tracing the scene
//
void HelloVulkan::raytrace(const VkCommandBuffer& cmdBuf, const vec4& clearColor)
{
	VulkanContext->Debug.beginLabel(cmdBuf, "Ray trace");
	// Initializing push constant values
	m_pcRay.clearColor     = clearColor;
	m_pcRay.mouseX = hostUBO.mouseX;
	m_pcRay.mouseY = hostUBO.mouseY;
	m_pcRay.hitIndex = 0;
	const VkExtent2D& size = takingScreenshot ? m_offscreenSize : m_size;
	m_pcRay.width = size.width;
	m_pcRay.height = size.height;
	m_pcRay.offsetX = m_batch.width * m_size.width;
	m_pcRay.offsetY = m_batch.height * m_size.height;

	auto& descriptorSets = RTPipeline->GetDescriptorSets();

	std::vector<VkDescriptorSet> descSets{descriptorSets[0]->DescriptorSet, descriptorSets[1]->DescriptorSet };
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RTPipeline->GetPipeline());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RTPipeline->GetPipelineLayout(), 0,
							(uint32_t)descSets.size(), descSets.data(), 0, nullptr);
	vkCmdPushConstants(cmdBuf, RTPipeline->GetPipelineLayout(),
					 RTPipeline->FetchPushConstant().stageFlags,
					 0, sizeof(PushConstantRay), &m_pcRay);

	vkCmdTraceRaysKHR(cmdBuf, &RTPipeline->GetRayGenRegion(), &RTPipeline->GetRayMissRegion(), &RTPipeline->GetRayHitRegion(), &m_callRegion, m_size.width, m_size.height, 1);


	VulkanContext->Debug.endLabel(cmdBuf);
}
