// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneSerializer.h"
#include "Origin/Core/ConsoleManager.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Renderer/Lighting/Lighting.hpp"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Project/Project.h"
#include "Origin/Renderer/Shader.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Serializer/Serializer.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin {
#define WRITE_FIELD_TYPE(FieldType, Type)\
case ScriptFieldType::FieldType:\
{\
out << scriptField.GetValue<Type>();\
break;\
}

#define	READ_FIELD_TYPE(FieldType, Type)\
case ScriptFieldType::FieldType:\
{\
Type data = scriptField["Data"].as<Type>();\
fieldInstance.SetValue(data);\
break;\
}

static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
{
	switch (bodyType)
	{
	case Rigidbody2DComponent::BodyType::Static: return "Static";
	case Rigidbody2DComponent::BodyType::Dynamic: return "Dynamic";
	case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
	}

	OGN_CORE_ASSERT(false, "Unknown body type");
	return {};
}

static Rigidbody2DComponent::BodyType Rigidbody2DBodyTypeFromString(const std::string& bodyTypeString)
{
	if (bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::Static;
	if (bodyTypeString == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
	if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

	OGN_CORE_ASSERT(false, "Unknown body type");
	return Rigidbody2DComponent::BodyType::Static;
}

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
	: m_Scene(scene)
{
}

static void SerializeEntity(YAML::Emitter& out, Entity entity)
{
	OGN_PROFILER_FUNCTION();

	OGN_CORE_ASSERT(entity.HasComponent<IDComponent>(), "");
	auto &idc = entity.GetComponent<IDComponent>();
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
	out << YAML::Key << "Type" << YAML::Value << Utils::EntityTypeToString(idc.Type);
	out << YAML::Key << "Parent" << YAML::Value << idc.Parent;

	out << YAML::Key << "Children";
	out << YAML::BeginSeq;
	for (const UUID child_uuid : idc.Children)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "ID" << YAML::Value << child_uuid;
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	if (entity.HasComponent<TagComponent>())
	{
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap; // TagComponent
		auto tag = entity.GetComponent<TagComponent>().Tag;
		out << YAML::Key << "Tag" << YAML::Value << tag;
		out << YAML::EndMap; // TagComponent
	}

	if (entity.HasComponent<SpriteAnimationComponent>())
	{
		SpriteAnimationComponent &ac = entity.GetComponent<SpriteAnimationComponent>();

		out << YAML::Key << "SpriteAnimationComponent";
		out << YAML::BeginMap; // SpriteAnimationComponent
		out << YAML::Key << "States";
		out << YAML::BeginSeq; // States

		auto states = ac.State->GetAnimationState();

		for (const auto &state : states)
		{
			const std::shared_ptr<SpriteAnimation> &currentAnim = state.second;
			out << YAML::BeginMap; // Name
			out << YAML::Key << "Name" << state.first;
			out << YAML::Key << "Looping" << YAML::Value << currentAnim->Looping;
			out << YAML::Key << "MaxFrame" << YAML::Value << currentAnim->MaxFrame;
			out << YAML::Key << "Speed" << YAML::Value << currentAnim->Speed;
			out << YAML::Key << "Frames" << YAML::Value;

			out << YAML::BeginSeq; // Frames
			for (const auto &frame : currentAnim->AnimFrames)
			{
				out << YAML::BeginMap; // ID
				out << YAML::Key << "ID" << YAML::Value <<frame.Handle;
				out << YAML::Key << "FrameBegin" << YAML::Value <<frame.FrameBegin;
				out << YAML::Key << "FrameEnd" << YAML::Value <<frame.FrameEnd;
				out << YAML::Key << "UV0" << YAML::Value <<frame.UV0;
				out << YAML::Key << "UV1" << YAML::Value <<frame.UV1;
				out << YAML::EndMap; //!ID
			}
			out << YAML::EndSeq; //!Frames

			out << YAML::EndMap; // !Name
		}

		out << YAML::EndSeq; // !States
		out << YAML::EndMap; // !SpriteAnimationComponent
	}

	if (entity.HasComponent<UIComponent>())
	{
		auto ui = entity.GetComponent<UIComponent>();
		out << YAML::Key << "UIComponent";
		out << YAML::BeginMap; // UIComponent

		out << YAML::Key << "Components" << YAML::BeginSeq; // Components

		for (int i = 0; i < ui.Components.size(); i++)
		{
			out << YAML::BeginMap;
			if (UIData<TextComponent> *text = ui.GetComponent<TextComponent>(ui.Components[i]->Name))
			{
				out << YAML::Key << "Type" << "TextComponent";
				out << YAML::Key << "Name" << text->Name;
				out << YAML::Key << "Anchor" << (int)text->AnchorType;
				out << YAML::Key << "FontHandle" << text->Component.FontHandle;
				out << YAML::Key << "TextString" << text->Component.TextString;
				out << YAML::Key << "LineSpacing" << text->Component.LineSpacing;
				out << YAML::Key << "Kerning" << text->Component.Kerning;
				out << YAML::Key << "Color" << text->Component.Color;
				out << YAML::Key << "Translation" << text->Transform.WorldTranslation;
				out << YAML::Key << "Rotation" << text->Transform.WorldRotation;
				out << YAML::Key << "Scale" << text->Transform.WorldScale;
			}
			else if (UIData<SpriteRenderer2DComponent> *sprite = ui.GetComponent<SpriteRenderer2DComponent>(ui.Components[i]->Name))
			{
				out << YAML::Key << "Type" << "SpriteRenderer2DComponent";
				out << YAML::Key << "Name" << sprite->Name;
				out << YAML::Key << "Anchor" << (int)sprite->AnchorType;
				out << YAML::Key << "TextureHandle" << sprite->Component.Texture;
				out << YAML::Key << "UV0" << sprite->Component.UV0;
				out << YAML::Key << "UV1" << sprite->Component.UV1;
				out << YAML::Key << "TillingFactor" << sprite->Component.TillingFactor;
				out << YAML::Key << "Color" << sprite->Component.Color;
				out << YAML::Key << "FlipX" << sprite->Component.FlipX;
				out << YAML::Key << "FlipY" << sprite->Component.FlipY;
				out << YAML::Key << "Translation" << sprite->Transform.WorldTranslation;
				out << YAML::Key << "Rotation" << sprite->Transform.WorldRotation;
				out << YAML::Key << "Scale" << sprite->Transform.WorldScale;
			}
			out << YAML::EndMap;
		}

		out << YAML::EndSeq; // !Components

		out << YAML::EndMap; // !UIComponent
	}

	if (entity.HasComponent<RigidbodyComponent>())
	{
		out << YAML::Key << "RigidbodyComponent";
		out << YAML::BeginMap; // RigidbodyComponent
		const auto &rigidbody = entity.GetComponent<RigidbodyComponent>();

		out << YAML::Key << "Mass" << YAML::Value << rigidbody.Mass;
		out << YAML::Key << "CenterMass" << YAML::Value << rigidbody.CenterMass;
		out << YAML::Key << "Offset" << YAML::Value << rigidbody.Offset;
		out << YAML::Key << "UseGravity" << YAML::Value << rigidbody.UseGravity;
		out << YAML::Key << "RotateX" << YAML::Value << rigidbody.RotateX;
		out << YAML::Key << "RotateY" << YAML::Value << rigidbody.RotateY;
		out << YAML::Key << "RotateZ" << YAML::Value << rigidbody.RotateZ;
		out << YAML::Key << "MoveX" << YAML::Value << rigidbody.MoveX;
		out << YAML::Key << "MoveY" << YAML::Value << rigidbody.MoveY;
		out << YAML::Key << "MoveZ" << YAML::Value << rigidbody.MoveZ;
		out << YAML::Key << "IsStatic" << YAML::Value << rigidbody.IsStatic;
		out << YAML::EndMap; // !Rigidbody
	}

	if (entity.HasComponent<BoxColliderComponent>())
	{
		out << YAML::Key << "BoxColliderComponent";
		out << YAML::BeginMap; // BoxColliderComponent
		const auto &bc = entity.GetComponent<BoxColliderComponent>();
		out << YAML::Key << "Size" << YAML::Value << bc.Scale;
		out << YAML::Key << "Restitution" << YAML::Value << bc.Restitution;
		out << YAML::Key << "Friction" << YAML::Value << bc.Friction;
		out << YAML::EndMap; // !BoxColliderComponent
	}

	if (entity.HasComponent<SphereColliderComponent>())
	{
		out << YAML::Key << "SphereColliderComponent";
		out << YAML::BeginMap;
		const auto &sc = entity.GetComponent<SphereColliderComponent>();
		out << YAML::Key << "Radius" << sc.Radius;
		out << YAML::Key << "Restitution" << sc.Restitution;
		out << YAML::Key << "Friction" << YAML::Value << sc.Friction;
		out << YAML::EndMap;
	}

	if (entity.HasComponent<CapsuleColliderComponent>())
	{
		out << YAML::Key << "CapsuleColliderComponent";
		out << YAML::BeginMap;
		const auto &sc = entity.GetComponent<CapsuleColliderComponent>();
		out << YAML::Key << "Radius" << sc.Radius;
		out << YAML::Key << "HalfHeight" << sc.HalfHeight;
		out << YAML::Key << "Restitution" << sc.Restitution;
		out << YAML::Key << "Friction" << YAML::Value << sc.Friction;
		out << YAML::EndMap;
	}

	if (entity.HasComponent<ScriptComponent>())
	{
		out << YAML::Key << "ScriptComponent";
		out << YAML::BeginMap; // ScriptComponent;

		auto& sc = entity.GetComponent<ScriptComponent>();
		out << YAML::Key << "ClassName" << YAML::Value << sc.ClassName;

		// Fields
		const std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClassesByName(sc.ClassName);

		if (entityClass)
		{
			const auto& fields = entityClass->GetFields();
			if (!fields.empty())
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityFields.find(name) == entityFields.end())
					{
						OGN_CORE_ERROR("[SceneSerializer] {} SCRIPT FIELDS NOT FOUND", name);
						PUSH_CONSOLE_ERROR("[SceneSerializer] {} SCRIPT FIELDS NOT FOUND", name);
						continue;
					}

					out << YAML::BeginMap; // Fields
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);
					out << YAML::Key << "Data" << YAML::Value;

					ScriptFieldInstance& scriptField = entityFields.at(name);
					switch (field.Type)
					{
						WRITE_FIELD_TYPE(Float, float);
						WRITE_FIELD_TYPE(Double, double);
						WRITE_FIELD_TYPE(Bool, bool);
						WRITE_FIELD_TYPE(Char, char);
						WRITE_FIELD_TYPE(Byte, int8_t);
						WRITE_FIELD_TYPE(Short, int16_t);
						WRITE_FIELD_TYPE(Int, int32_t);
						WRITE_FIELD_TYPE(Long, int64_t);
						WRITE_FIELD_TYPE(UByte, uint8_t);
						WRITE_FIELD_TYPE(UShort, uint16_t);
						WRITE_FIELD_TYPE(UInt, uint32_t);
						WRITE_FIELD_TYPE(ULong, uint64_t);
						WRITE_FIELD_TYPE(Vector2, glm::vec2);
						WRITE_FIELD_TYPE(Vector3, glm::vec3);
						WRITE_FIELD_TYPE(Vector4, glm::vec4);
						WRITE_FIELD_TYPE(Entity, uint64_t);
					}

					out << YAML::EndMap; // !Fields
				}
				out << YAML::EndSeq;
			}
		}
		out << YAML::EndMap; // !ScriptComponent;
	}

	if (entity.HasComponent<AudioComponent>())
	{
		const auto& ac = entity.GetComponent<AudioComponent>();

		out << YAML::Key << "AudioComponent";
		out << YAML::BeginMap; // AudioComponent

		if (ac.Audio != 0)
		{
			out << YAML::Key << "AudioHandle" << YAML::Value << ac.Audio;
			out << YAML::Key << "Name" << ac.Name;
			out << YAML::Key << "Volume" << YAML::Value << ac.Volume;
			out << YAML::Key << "Pitch" << YAML::Value << ac.Pitch;
			out << YAML::Key << "Panning" << YAML::Value << ac.Panning;
			out << YAML::Key << "MinDistance" << YAML::Value << ac.MinDistance;
			out << YAML::Key << "MaxDistance" << YAML::Value << ac.MaxDistance;
			out << YAML::Key << "Looping" << YAML::Value << ac.Looping;
			out << YAML::Key << "Spatial" << YAML::Value << ac.Spatializing;
			out << YAML::Key << "PlayAtStart" << YAML::Value << ac.PlayAtStart;
			out << YAML::Key << "Overlapping" << YAML::Value << ac.Overlapping;
		}
			
		out << YAML::EndMap; // AudioComponent
	}

	if (entity.HasComponent<TransformComponent>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap; // TransformComponent

		const auto& tc = entity.GetComponent<TransformComponent>();
		out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
		out << YAML::Key << "Visible" << tc.Visible;

		out << YAML::EndMap; // !TransformComponent
	}

	if (entity.HasComponent<DirectionalLightComponent>())
	{
		out << YAML::Key << "DirectionalLightComponent";
		out << YAML::BeginMap; // DirectionalLightComponent

		const auto &dlc = entity.GetComponent<DirectionalLightComponent>();
		Ref<DirectionalLight> light = std::static_pointer_cast<DirectionalLight>(dlc.Light);
		out << YAML::Key << "Direction" << light->color;
		out << YAML::Key << "Color" << light->color;

		out << YAML::EndMap; // !DirectionalLightComponent
	}

	if (entity.HasComponent<CameraComponent>())
	{
		out << YAML::Key << "CameraComponent";
		out << YAML::BeginMap; // CameraComponent

		const auto& cameraComponent = entity.GetComponent<CameraComponent>();
		const auto& camera = cameraComponent.Camera;

		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap; // Camera
		out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.GetProjectionType());
		out << YAML::Key << "AspectRatioType" << YAML::Value << static_cast<int>(camera.GetAspectRatioType());
		out << YAML::Key << "FOV" << YAML::Value << camera.GetFOV();
		out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetNear();
		out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetFar();
		out << YAML::Key << "OrthoScale" << YAML::Value << camera.GetOrthoScale();
		out << YAML::Key << "OrthoNear" << YAML::Value << camera.GetOrthoNear();
		out << YAML::Key << "OrthoFar" << YAML::Value << camera.GetOrthoFar();
		out << YAML::Key << "ViewportSize" << YAML::Value << camera.GetViewportSize();
		out << YAML::EndMap; // !Camera

		out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;

		out << YAML::EndMap; // !CameraComponent
	}

	if (entity.HasComponent<MeshComponent>())
	{
		out << YAML::Key << "MeshComponent";
		out << YAML::BeginMap; // MeshComponent

		const MeshComponent sc = entity.GetComponent<MeshComponent>();
		out << YAML::Key << "Name" << sc.Name;
		out << YAML::Key << "HModel" << sc.HModel;
		out << YAML::EndMap; // !MeshComponent
	}

	if (entity.HasComponent<EnvironmentMap>())
	{
		EnvironmentMap &env_comp = entity.GetComponent<EnvironmentMap>();
		out << YAML::Key << "EnvironmentMap";
		out << YAML::BeginMap;
		out << YAML::Key << "Blur Factor" << env_comp.blur_factor;
		out << YAML::EndMap;
	}

	if (entity.HasComponent<AudioListenerComponent>())
	{
		out << YAML::Key << "AudioListenerComponent";
		out << YAML::BeginMap; // AudioListenerComponent

		const auto& al = entity.GetComponent<AudioListenerComponent>();
		out << YAML::Key << "Enable" << YAML::Value << al.Enable;

		out << YAML::EndMap; // !AudioListenerComponent
	}

	if (entity.HasComponent<TextComponent>())
	{
		out << YAML::Key << "TextComponent";
		out << YAML::BeginMap; // TextComponent
		const auto& textComponent = entity.GetComponent<TextComponent>();
		out << YAML::Key << "Handle" << YAML::Value << textComponent.FontHandle;
		out << YAML::Key << "Text" << YAML::Value << textComponent.TextString;
		out << YAML::Key << "Color" << YAML::Value << textComponent.Color;
		out << YAML::Key << "Kerning" << YAML::Value << textComponent.Kerning;
		out << YAML::Key << "LineSpacing" << YAML::Value << textComponent.LineSpacing;
		out << YAML::Key << "ScreenSpace" << YAML::Value << textComponent.ScreenSpace;

		out << YAML::EndMap; // !TextComponent
	}

	if (entity.HasComponent<ParticleComponent>())
	{
		out << YAML::Key << "ParticleComponent";
		out << YAML::BeginMap; // ParticleComponent
		const auto& particleComponent = entity.GetComponent<ParticleComponent>();
		out << YAML::Key << "Velocity" << YAML::Value << particleComponent.Velocity;
		out << YAML::Key << "VelocityVariation" << YAML::Value << particleComponent.VelocityVariation;
		out << YAML::Key << "Rotation" << YAML::Value << particleComponent.Rotation;
		out << YAML::Key << "ColorBegin" << YAML::Value << particleComponent.ColorBegin;
		out << YAML::Key << "ColorEnd" << YAML::Value << particleComponent.ColorEnd;
		out << YAML::Key << "SizeBegin" << YAML::Value << particleComponent.SizeBegin;
		out << YAML::Key << "SizeEnd" << YAML::Value << particleComponent.SizeEnd;
		out << YAML::Key << "SizeVariation" << YAML::Value << particleComponent.SizeVariation;
		out << YAML::Key << "ZAxis" << YAML::Value << particleComponent.ZAxis;
		out << YAML::Key << "LifeTime" << YAML::Value << particleComponent.LifeTime;

		out << YAML::EndMap; // !ParticleComponent
	}

	if (entity.HasComponent<SpriteRenderer2DComponent>())
	{
		out << YAML::Key << "SpriteRenderer2DComponent";
		out << YAML::BeginMap; // SpriteRenderer2DComponent

		const auto& src = entity.GetComponent<SpriteRenderer2DComponent>();
		out << YAML::Key << "Color" << YAML::Value << src.Color;
		if (src.Texture != 0)
		{
			out << YAML::Key << "Handle" << YAML::Value << src.Texture;
			out << YAML::Key << "UV0" << YAML::Value << src.UV0;
			out << YAML::Key << "UV1" << YAML::Value << src.UV1;
			out << YAML::Key << "TillingFactor" << YAML::Value << src.TillingFactor;
			out << YAML::Key << "FlipX" << YAML::Value << src.FlipX;
			out << YAML::Key << "FlipY" << YAML::Value << src.FlipY;
		}

		out << YAML::EndMap; // !SpriteRenderer2DComponent
	}

	if (entity.HasComponent<CircleRendererComponent>())
	{
		out << YAML::Key << "CircleRendererComponent";
		out << YAML::BeginMap; // CircleRendererComponent

		const auto& src = entity.GetComponent<CircleRendererComponent>();
		out << YAML::Key << "Color" << YAML::Value << src.Color;
		out << YAML::Key << "Fade" << YAML::Value << src.Fade;
		out << YAML::Key << "Thickness" << YAML::Value << src.Thickness;

		out << YAML::EndMap; // !CircleRendererComponent
	}

	if (entity.HasComponent<Rigidbody2DComponent>())
	{
		out << YAML::Key << "Rigidbody2DComponent";
		out << YAML::BeginMap; // Rigidbody2DComponent

		const auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2d.Type);
		out << YAML::Key << "Mass" << YAML::Value << rb2d.Mass;
		out << YAML::Key << "LinearDamping" << YAML::Value << rb2d.LinearDamping;
		out << YAML::Key << "AngularDamping" << YAML::Value << rb2d.AngularDamping;
		out << YAML::Key << "RotationalInertia" << YAML::Value << rb2d.RotationalInertia;
		out << YAML::Key << "GravityScale" << YAML::Value << rb2d.GravityScale;
		out << YAML::Key << "MassCenter" << YAML::Value << rb2d.MassCenter;
		out << YAML::Key << "EnableSleep" << YAML::Value << rb2d.EnableSleep;
		out << YAML::Key << "IsAwake" << YAML::Value << rb2d.IsAwake;
		out << YAML::Key << "IsBullet" << YAML::Value << rb2d.IsBullet;
		out << YAML::Key << "IsEnabled" << YAML::Value << rb2d.IsEnabled;
		out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;


		out << YAML::EndMap; // !Rigidbody2DComponent
	}

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		out << YAML::Key << "BoxCollider2DComponent";
		out << YAML::BeginMap; // BoxCollider2DComponent;

		const auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		out << YAML::Key << "Group" << YAML::Value << bc2d.Group;
		out << YAML::Key << "Offset" << YAML::Value << bc2d.Offset;
		out << YAML::Key << "Size" << YAML::Value << bc2d.Size;

		out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
		out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
		out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;
		out << YAML::Key << "IsSensor" << YAML::Value << bc2d.IsSensor;

		out << YAML::EndMap; // !BoxCollider2DComponent
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		out << YAML::Key << "CircleCollider2DComponent";
		out << YAML::BeginMap; // CircleCollider2DComponent;

		const auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
		out << YAML::Key << "Group" << YAML::Value << cc2d.Group;
		out << YAML::Key << "Offset" << YAML::Value << cc2d.Offset;
		out << YAML::Key << "Radius" << YAML::Value << cc2d.Radius;

		out << YAML::Key << "Density" << YAML::Value << cc2d.Density;
		out << YAML::Key << "Friction" << YAML::Value << cc2d.Friction;
		out << YAML::Key << "Restitution" << YAML::Value << cc2d.Restitution;
		out << YAML::Key << "IsSensor" << YAML::Value << cc2d.IsSensor;

		out << YAML::EndMap; // !CircleCollider2DComponent
	}

	if (entity.HasComponent<RevoluteJoint2DComponent>())
	{
		out << YAML::Key << "RevoluteJoint2DComponent";
		out << YAML::BeginMap; // RevoluteJoint2DComponent;

		const auto& rjc = entity.GetComponent<RevoluteJoint2DComponent>();
		out << YAML::Key << "Connected Body ID" << rjc.ConnectedBodyID;
		// Angles in degrees
		out << YAML::Key << "Anchor"  << rjc.AnchorPoint;
		out << YAML::Key << "AnchorB" << rjc.AnchorPointB;
		out << YAML::Key << "EnableLimit" << rjc.EnableLimit;
		out << YAML::Key << "MaxMotorTorque" << rjc.MaxMotorTorque;
		out << YAML::Key << "EnableMotor" << rjc.EnableMotor;
		out << YAML::Key << "MotorSpeed" << rjc.MotorSpeed;
		out << YAML::Key << "DampingRatio" << rjc.SpringDampingRatio;

		out << YAML::EndMap; // !RevoluteJoint2DComponent;
	}

	if (entity.HasComponent<NativeScriptComponent>())
	{
		auto& nsc = entity.GetComponent<NativeScriptComponent>();
	}
	out << YAML::EndMap; // !Entity
}

void SceneSerializer::Serialize(const std::filesystem::path &path)
{
	std::string filepath = path.generic_string();
	size_t pos = filepath.find_last_of(".");
	if (pos != std::string::npos)
	{
		std::string extension = filepath.substr(pos);
		if (extension == ".org")
		{
			filepath.erase(pos);
		}
	}

	filepath += ".org";

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << path.filename().string();
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	auto view = m_Scene->m_Registry.view<IDComponent>();
	for (auto [e, id] : view.each())
	{
		Entity entity = { e, m_Scene.get() };
		if (!entity.IsValid())
			return;

		SerializeEntity(out, entity);
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
	fout.close();
	OGN_CORE_INFO("[SceneSerializer] Scene Serialized in {}", filepath);
	PUSH_CONSOLE_INFO("[SceneSerializer] Scene Serialized in {}", filepath);
}

void SceneSerializer::SerializeRuntime(const std::filesystem::path& filepath)
{
}

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
{
	OGN_PROFILER_FUNCTION();

	std::ifstream stream(filepath.string());
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Scene"])
		return false;

	auto sceneName = data["Scene"].as<std::string>();
	OGN_CORE_TRACE("[SceneSerializer] Deserializing scene '{}'", sceneName);
	PUSH_CONSOLE_INFO("[SceneSerializer] Deserializing scene '{}'", sceneName);

	if (YAML::Node entities = data["Entities"])
	{
		for (YAML::iterator::value_type entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();
			std::string name = entity["TagComponent"]["Tag"].as<std::string>();
			EntityType type = Utils::EntityTypeStringToType(entity["Type"].as<std::string>());
			Entity deserialized_entity = EntityManager::CreateEntityWithUUID(uuid, name, EntityType::Entity, m_Scene.get());

			IDComponent &idc = deserialized_entity.GetComponent<IDComponent>();
			idc.Parent = entity["Parent"].as<uint64_t>();

			if (YAML::Node children = entity["Children"])
			{
				for (YAML::detail::iterator_value child : children)
				{
					u64 child_uuid = child["ID"].as<u64>();
					idc.AddChild(child_uuid);
				}
			}

			if (YAML::Node transform_component = entity["TransformComponent"])
			{
				TransformComponent &tc = deserialized_entity.GetComponent<TransformComponent>();
				tc.Translation = transform_component["Translation"].as<glm::vec3>();
				tc.Rotation = transform_component["Rotation"].as<glm::quat>();
				tc.Scale = transform_component["Scale"].as<glm::vec3>();
				tc.Visible = transform_component["Visible"].as<bool>();
			}

			if (YAML::Node audio_listener_component = entity["AudioListenerComponent"])
			{
				AudioListenerComponent &al = deserialized_entity.AddComponent<AudioListenerComponent>();
				al.Enable = audio_listener_component["Enable"].as<bool>();
			}

			if (YAML::Node sprite_animation_component = entity["SpriteAnimationComponent"])
			{
				SpriteAnimationComponent &ac = deserialized_entity.AddComponent<SpriteAnimationComponent>();
				if (auto states = sprite_animation_component["States"])
				{
					for (auto state : states)
					{
						std::string name = state["Name"].as<std::string>();
						std::shared_ptr<SpriteAnimation> newAnim = std::make_shared<SpriteAnimation>();
						newAnim->Looping = state["Looping"].as<bool>();
						newAnim->MaxFrame = state["MaxFrame"].as<int>();
						newAnim->Speed = state["Speed"].as<float>();

						for (auto frames : state["Frames"])
						{
							AssetHandle handle = frames["ID"].as<uint64_t>();
							SpriteAnimationFrame frame(handle);
							frame.FrameBegin = frames["FrameBegin"].as<int>();
							frame.FrameEnd = frames["FrameEnd"].as<int>();
							frame.UV0 = frames["UV0"].as<glm::vec2>();
							frame.UV1 = frames["UV1"].as<glm::vec2>();
							newAnim->AddFrame(frame);
						}

						ac.State->AddState(name);
						ac.State->AddAnimation(newAnim);
					}
				}
			}

			if (YAML::Node ui_component = entity["UIComponent"])
			{
				UIComponent &ui = deserialized_entity.AddComponent<UIComponent>();

				for (auto comp : ui_component["Components"])
				{
					std::string types = comp["Type"].as<std::string>();
					if (types == "TextComponent")
					{
						UIData<TextComponent> component;
						component.AnchorType = (BaseUIData::Anchor) comp["Anchor"].as<int>();
						component.Component.TextString = comp["TextString"].as<std::string>();
						component.Component.Kerning = comp["Kerning"].as<float>();
						component.Component.LineSpacing = comp["LineSpacing"].as<float>();
						AssetHandle fontHandle = comp["FontHandle"].as<uint64_t>();
						component.Component.FontHandle = fontHandle;
						if (fontHandle)
						{
							AssetManager::GetAsset<Font>(fontHandle);
						}
						component.Component.Color = comp["Color"].as<glm::vec4>();
						component.Transform.WorldTranslation = comp["Translation"].as<glm::vec3>();
						component.Transform.WorldRotation = comp["Rotation"].as<glm::quat>();
						component.Transform.WorldScale = comp["Scale"].as<glm::vec3>();

						std::string name = comp["Name"].as<std::string>();
						ui.AddComponent<TextComponent>(name, component);
					}
					else if (types == "SpriteRenderer2DComponent")
					{
						UIData<SpriteRenderer2DComponent> component;
						component.AnchorType = (BaseUIData::Anchor) comp["Anchor"].as<int>();
						component.Component.UV0 = comp["UV0"].as<glm::vec2>();
						component.Component.UV1 = comp["UV1"].as<glm::vec2>();
						component.Component.TillingFactor = comp["TillingFactor"].as<glm::vec2>();
						component.Component.FlipX = comp["FlipX"].as<bool>();
						component.Component.FlipY = comp["FlipY"].as<bool>();
						AssetHandle textureHandle = comp["TextureHandle"].as<uint64_t>();
						component.Component.Texture = textureHandle;
						component.Component.Color = comp["Color"].as<glm::vec4>();
						component.Transform.WorldTranslation = comp["Translation"].as<glm::vec3>();
						component.Transform.WorldRotation = comp["Rotation"].as<glm::quat>();
						component.Transform.WorldScale = comp["Scale"].as<glm::vec3>();

						std::string name = comp["Name"].as<std::string>();
						ui.AddComponent<SpriteRenderer2DComponent>(name, component);
					}
				}
			}

			if (YAML::Node audio_component = entity["AudioComponent"])
			{
				AudioComponent &ac = deserialized_entity.AddComponent<AudioComponent>();
				if (audio_component["AudioHandle"])
				{
					ac.Audio = audio_component["AudioHandle"].as<uint64_t>();
					ac.Name = audio_component["Name"].as<std::string>();
					ac.Volume = audio_component["Volume"].as<float>();
					ac.Pitch = audio_component["Pitch"].as<float>();
					ac.Panning = audio_component["Panning"].as<float>();
					ac.MinDistance = audio_component["MinDistance"].as<float>();
					ac.MaxDistance = audio_component["MaxDistance"].as<float>();
					ac.Looping = audio_component["Looping"].as<bool>();
					ac.Spatializing = audio_component["Spatial"].as<bool>();
					ac.PlayAtStart = audio_component["PlayAtStart"].as<bool>();
					ac.Overlapping = audio_component["Overlapping"].as<bool>();
					Ref<FmodSound> fmod_sound = AssetManager::GetAsset<FmodSound>(ac.Audio);
					fmod_sound->SetVolume(ac.Volume);
					if (ac.Looping)
						fmod_sound->SetMode(FMOD_LOOP_NORMAL);
					fmod_sound->SetPitch(ac.Pitch);
				}
			}

			if (YAML::Node camera_component = entity["CameraComponent"])
			{
				CameraComponent &cc = deserialized_entity.AddComponent<CameraComponent>();
				const auto& cameraProps = camera_component["Camera"];
				glm::vec2 viewportSize = cameraProps["ViewportSize"].as<glm::vec2>();
				float fov = cameraProps["FOV"].as<float>();
				float nearClip = cameraProps["PerspectiveNear"].as<float>();
				float farClip = cameraProps["PerspectiveFar"].as<float>();
				cc.Camera.InitPerspective(fov, viewportSize.x / viewportSize.y, nearClip, farClip);

				float orthoScale = cameraProps["OrthoScale"].as<float>();
				nearClip = cameraProps["OrthoNear"].as<float>();
				farClip = cameraProps["OrthoFar"].as<float>();
				cc.Camera.InitOrthographic(orthoScale, nearClip, farClip);
				cc.Camera.SetProjectionType(static_cast<ProjectionType>(cameraProps["ProjectionType"].as<int>()));
				cc.Camera.SetAspectRatioType(static_cast<AspectRatioType>(cameraProps["AspectRatioType"].as<int>()));
                cc.Camera.SetViewportSize(static_cast<u32>(viewportSize.x), static_cast<u32>(viewportSize.y));
				cc.Primary = camera_component["Primary"].as<bool>();
			}

			if (YAML::Node mesh_component = entity["MeshComponent"])
			{
				MeshComponent &mc = deserialized_entity.AddComponent<MeshComponent>();
				mc.Name = mesh_component["Name"].as<std::string>();
				mc.HModel = mesh_component["HModel"].as<uint64_t>();
				mc.blend_space.SetModel(AssetManager::GetAsset<Model>(mc.HModel));
			}

			if (YAML::Node env_component = entity["EnvironmentMap"])
			{
				EnvironmentMap &env_map_comp = deserialized_entity.AddComponent<EnvironmentMap>();
				env_map_comp.blur_factor = env_component["Blur Factor"].as<float>();
			}

			if (YAML::Node directional_light_component = entity["DirectionalLightComponent"])
			{
				DirectionalLightComponent &dlc = deserialized_entity.AddComponent<DirectionalLightComponent>();
				Ref<DirectionalLight> light = std::static_pointer_cast<DirectionalLight>(dlc.Light);
				light->color = directional_light_component["Color"].as<glm::vec3>();
				light->direction = directional_light_component["Direction"].as<glm::vec3>();
			}

			if (YAML::Node particle_component = entity["ParticleComponent"])
			{
				ParticleComponent &pc = deserialized_entity.AddComponent<ParticleComponent>();
				pc.Velocity = particle_component["Velocity"].as<glm::vec3>();
				pc.VelocityVariation = particle_component["VelocityVariation"].as<glm::vec3>();
				pc.Rotation = particle_component["Rotation"].as<glm::vec3>();
				pc.ColorBegin = particle_component["ColorBegin"].as<glm::vec4>();
				pc.ColorEnd = particle_component["ColorEnd"].as<glm::vec4>();
				pc.SizeBegin = particle_component["SizeBegin"].as<float>();
				pc.SizeEnd = particle_component["SizeEnd"].as<float>();
				pc.SizeVariation = particle_component["SizeVariation"].as<float>();
				pc.ZAxis = particle_component["ZAxis"].as<float>();
				pc.LifeTime = particle_component["LifeTime"].as<float>();
			}

			if (YAML::Node sprite_renderer_2d_component = entity["SpriteRenderer2DComponent"])
			{
				SpriteRenderer2DComponent &src = deserialized_entity.AddComponent<SpriteRenderer2DComponent>();
				src.Color = sprite_renderer_2d_component["Color"].as<glm::vec4>();
				if(sprite_renderer_2d_component["Handle"])
				{
					src.Texture = sprite_renderer_2d_component["Handle"].as<uint64_t>();
					src.UV0 = sprite_renderer_2d_component["UV0"].as<glm::vec2>();
					src.UV1 = sprite_renderer_2d_component["UV1"].as<glm::vec2>();
					src.TillingFactor = sprite_renderer_2d_component["TillingFactor"].as<glm::vec2>();
				}
			}

			if (YAML::Node circle_renderer_component = entity["CircleRendererComponent"])
			{
				CircleRendererComponent &src = deserialized_entity.AddComponent<CircleRendererComponent>();
				src.Color = circle_renderer_component["Color"].as<glm::vec4>();
				src.Fade = circle_renderer_component["Fade"].as<float>();
				src.Thickness = circle_renderer_component["Thickness"].as<float>();
			}

			if (YAML::Node rigid_body_component = entity["Rigidbody2DComponent"])
			{
				Rigidbody2DComponent &rb2d = deserialized_entity.AddComponent<Rigidbody2DComponent>();
				rb2d.Type = Rigidbody2DBodyTypeFromString(rigid_body_component["BodyType"].as<std::string>());
				rb2d.Mass = rigid_body_component["Mass"].as<float>();
				rb2d.LinearDamping = rigid_body_component["LinearDamping"].as<float>();
				rb2d.AngularDamping = rigid_body_component["AngularDamping"].as<float>();
				rb2d.RotationalInertia = rigid_body_component["RotationalInertia"].as<float>();
				rb2d.GravityScale = rigid_body_component["GravityScale"].as<float>();
				rb2d.MassCenter = rigid_body_component["MassCenter"].as<glm::vec2>();
				rb2d.EnableSleep = rigid_body_component["EnableSleep"].as<bool>();
				rb2d.IsAwake = rigid_body_component["IsAwake"].as<bool>();
				rb2d.IsBullet = rigid_body_component["IsBullet"].as<bool>();
				rb2d.IsEnabled = rigid_body_component["IsEnabled"].as<bool>();
				rb2d.FixedRotation = rigid_body_component["FixedRotation"].as<bool>();
			}

			if (YAML::Node box_collider_2d_component = entity["BoxCollider2DComponent"])
			{
				BoxCollider2DComponent &bc2d = deserialized_entity.AddComponent<BoxCollider2DComponent>();
				bc2d.Group = box_collider_2d_component["Group"].as<int>();
				bc2d.Offset = box_collider_2d_component["Offset"].as<glm::vec2>();
				bc2d.Size = box_collider_2d_component["Size"].as<glm::vec2>();
				bc2d.Density = box_collider_2d_component["Density"].as<float>();
				bc2d.Friction = box_collider_2d_component["Friction"].as<float>();
				bc2d.Restitution = box_collider_2d_component["Restitution"].as<float>();
				bc2d.IsSensor = box_collider_2d_component["IsSensor"].as<bool>();
			}

			if (YAML::Node circle_collider_2d_component = entity["CircleCollider2DComponent"])
			{
				CircleCollider2DComponent &cc2d = deserialized_entity.AddComponent<CircleCollider2DComponent>();
				cc2d.Group = circle_collider_2d_component["Group"].as<int>();
				cc2d.Offset = circle_collider_2d_component["Offset"].as<glm::vec2>();
				cc2d.Radius = circle_collider_2d_component["Radius"].as<float>();
				cc2d.Density = circle_collider_2d_component["Density"].as<float>();
				cc2d.Friction = circle_collider_2d_component["Friction"].as<float>();
				cc2d.Restitution = circle_collider_2d_component["Restitution"].as<float>();
				cc2d.IsSensor = circle_collider_2d_component["IsSensor"].as<bool>();
			}

			if (YAML::Node revolute_joint_2d_component = entity["RevoluteJoint2DComponent"])
			{
				RevoluteJoint2DComponent &rjc = deserialized_entity.AddComponent<RevoluteJoint2DComponent>();
				rjc.ConnectedBodyID = revolute_joint_2d_component["Connected Body ID"].as<uint64_t>();
				rjc.AnchorPoint = revolute_joint_2d_component["Anchor"].as<glm::vec2>();
				rjc.AnchorPointB = revolute_joint_2d_component["AnchorB"].as<glm::vec2>();

				rjc.EnableMotor = revolute_joint_2d_component["EnableMotor"].as<bool>();
				rjc.EnableLimit = revolute_joint_2d_component["EnableLimit"].as<bool>();
				rjc.MaxMotorTorque = revolute_joint_2d_component["MaxMotorTorque"].as<float>();
				rjc.MotorSpeed = revolute_joint_2d_component["MotorSpeed"].as<float>();
				rjc.SpringDampingRatio = revolute_joint_2d_component["DampingRatio"].as<float>();
			}

			if (YAML::Node text_component = entity["TextComponent"])
			{
				AssetHandle handle = text_component["Handle"].as<uint64_t>();
				if (AssetManager::IsAssetHandleValid(handle))
				{
					TextComponent &tc = deserialized_entity.AddComponent<TextComponent>();
					tc.FontHandle = handle;
					if (handle)
					{
						AssetManager::GetAsset<Font>(handle);
					}
					tc.TextString = text_component["Text"].as<std::string>();
					tc.Color = text_component["Color"].as<glm::vec4>();
					tc.Kerning = text_component["Kerning"].as<float>();
					tc.LineSpacing = text_component["LineSpacing"].as<float>();
					tc.ScreenSpace = text_component["ScreenSpace"].as<bool>();
				}
			}

			if (YAML::Node rigid_body_component = entity["RigidbodyComponent"])
			{
				auto &rigidbody = deserialized_entity.AddComponent<RigidbodyComponent>();
				rigidbody.Mass = rigid_body_component["Mass"].as<float>();
				rigidbody.CenterMass = rigid_body_component["CenterMass"].as<glm::vec3>();
				rigidbody.Offset = rigid_body_component["Offset"].as<glm::vec3>();
				rigidbody.UseGravity = rigid_body_component["UseGravity"].as<bool>();
				rigidbody.RotateX = rigid_body_component["RotateX"].as<bool>();
				rigidbody.RotateY = rigid_body_component["RotateY"].as<bool>();
				rigidbody.RotateZ = rigid_body_component["RotateZ"].as<bool>();
				rigidbody.MoveX = rigid_body_component["MoveX"].as<bool>();
				rigidbody.MoveY = rigid_body_component["MoveY"].as<bool>();
				rigidbody.MoveZ = rigid_body_component["MoveZ"].as<bool>();
				rigidbody.IsStatic = rigid_body_component["IsStatic"].as<bool>();
			}

			if (YAML::Node box_collider_component = entity["BoxColliderComponent"])
			{
				auto &boxCollider = deserialized_entity.AddComponent<BoxColliderComponent>();
				boxCollider.Scale = box_collider_component["Size"].as<glm::vec3>();
				boxCollider.Restitution = box_collider_component["Restitution"].as<float>();
				boxCollider.Friction = box_collider_component["Friction"].as<float>();
			}

			if (YAML::Node sphere_collider_component = entity["SphereColliderComponent"])
			{
				auto &sphereCollider = deserialized_entity.AddComponent<SphereColliderComponent>();
				sphereCollider.Radius = sphere_collider_component["Radius"].as<float>();
				sphereCollider.Restitution = sphere_collider_component["Restitution"].as<float>();
				sphereCollider.Friction = sphere_collider_component["Friction"].as<float>();
			}

			if (YAML::Node capsule_collider_component = entity["CapsuleColliderComponent"])
			{
				auto &capsuleCollider = deserialized_entity.AddComponent<CapsuleColliderComponent>();
				capsuleCollider.Radius = capsule_collider_component["Radius"].as<float>();
				capsuleCollider.HalfHeight = capsule_collider_component["HalfHeight"].as<float>();
				capsuleCollider.Restitution = capsule_collider_component["Restitution"].as<float>();
				capsuleCollider.Friction = capsule_collider_component["Friction"].as<float>();
			}
                
			if (YAML::Node script_component = entity["ScriptComponent"])
			{
				ScriptComponent & sc = deserialized_entity.AddComponent<ScriptComponent>();
				OGN_CORE_ASSERT(deserialized_entity.IsValid(), "SceneSerializer:  Entity is invalid");

				sc.ClassName = script_component["ClassName"].as<std::string>();

				if (YAML::Node scriptFields = script_component["ScriptFields"])
				{
					std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClassesByName(sc.ClassName);
					OGN_CORE_ASSERT(entityClass, "SceneSerializer: Entity Class is Invalid");

					if (entityClass)
					{
						const auto& fields = entityClass->GetFields();
						ScriptFieldMap& entityFields = ScriptEngine::GetScriptFieldMap(deserialized_entity);

						for (auto scriptField : scriptFields)
						{
							auto name = scriptField["Name"].as<std::string>();
							auto typeString = scriptField["Type"].as<std::string>();
							ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

							ScriptFieldInstance& fieldInstance = entityFields[name];

							if (fields.find(name) == fields.end())
								continue;

							fieldInstance.Field = fields.at(name);

							switch (type)
							{
								READ_FIELD_TYPE(Float, float);
								READ_FIELD_TYPE(Double, double);
								READ_FIELD_TYPE(Bool, bool);
								READ_FIELD_TYPE(Char, char);
								READ_FIELD_TYPE(Byte, int8_t);
								READ_FIELD_TYPE(Short, int16_t);
								READ_FIELD_TYPE(Int, int32_t);
								READ_FIELD_TYPE(Long, int64_t);
								READ_FIELD_TYPE(UByte, uint8_t);
								READ_FIELD_TYPE(UShort, uint16_t);
								READ_FIELD_TYPE(UInt, uint32_t);
								READ_FIELD_TYPE(ULong, uint64_t);
								READ_FIELD_TYPE(Vector2, glm::vec2);
								READ_FIELD_TYPE(Vector3, glm::vec3);
								READ_FIELD_TYPE(Vector4, glm::vec4);
								READ_FIELD_TYPE(Entity, uint64_t);
							}
						}
					}
				}
			}
		}
	}

	return true;
}

bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& filepath)
{
	return false;
}

void SceneSerializer::SerializeDeletedEntity(Entity entity, const std::filesystem::path &path)
{
	std::string filepath = path.generic_string();
	size_t pos = filepath.find_last_of(".");
	if (pos != std::string::npos)
	{
		std::string extension = filepath.substr(pos);
		if (extension == ".h")
		{
			filepath.erase(pos);
		}
	}

	filepath += ".org";

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "history" << YAML::Value << path.filename().string();
	SerializeEntity(out, entity);
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
	fout.close();
}

void SceneSerializer::DeserializeDeletedEntity(const std::filesystem::path& path)
{
	
}
}

