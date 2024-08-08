// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneSerializer.h"
#include "Origin/Core/ConsoleManager.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/Lighting.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Project/Project.h"
#include "Origin/Renderer/Shader.h"
#include "Origin/Renderer/ModelLoader.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Serializer/Serializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin
{
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
					out << YAML::Key << "Min" << YAML::Value <<frame.Min;
					out << YAML::Key << "Max" << YAML::Value <<frame.Max;
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
					out << YAML::Key << "Min" << sprite->Component.Min;
					out << YAML::Key << "Max" << sprite->Component.Max;
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
            out << YAML::Key << "Size" << YAML::Value << bc.Size;
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

		if (entity.HasComponent<StaticMeshComponent>())
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap; // StaticMeshComponent

			const StaticMeshComponent sc = entity.GetComponent<StaticMeshComponent>();
			out << YAML::Key << "Name" << sc.Name;
			out << YAML::Key << "Type" << (int)sc.mType;
			out << YAML::Key << "HMesh" << sc.HMesh;
			out << YAML::Key << "HMaterial" << sc.HMaterial;
			out << YAML::EndMap; // !StaticMeshComponent
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
				out << YAML::Key << "Min" << YAML::Value << src.Min;
				out << YAML::Key << "Max" << YAML::Value << src.Max;
				out << YAML::Key << "TillingFactor" << YAML::Value << src.TillingFactor;
				out << YAML::Key << "FlipX" << YAML::Value << src.FlipX;
				out << YAML::Key << "FlipY" << YAML::Value << src.FlipY;
			}

			out << YAML::EndMap; // !SpriteRenderer2DComponent
		}

		if (entity.HasComponent<LightComponent>())
		{
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			const auto& light = entity.GetComponent<LightComponent>().Light;
			out << YAML::Key << "Type" << YAML::Value << light->GetTypeString();

			switch (light->Type)
			{
#if 0
			case LightingType::Spot:
			{
				out << YAML::Key << "InnerConeAngle" << YAML::Value << light->InnerConeAngle;
				out << YAML::Key << "OuterConeAngle" << YAML::Value << light->OuterConeAngle;
				out << YAML::Key << "Exponent" << YAML::Value << light->Exponent;
				break;
			}
			case LightingType::Point:
			{
				out << YAML::Key << "Ambient" << YAML::Value << light->Ambient;
				out << YAML::Key << "Specular" << YAML::Value << light->Specular;
				break;
			}
#endif
			case LightingType::Directional:
			{
				out << YAML::Key << "Color" << YAML::Value << light->DirLightData.Color;
				out << YAML::Key << "Near" << YAML::Value << light->NearPlane;
				out << YAML::Key << "Far" << YAML::Value << light->FarPlane;
                out << YAML::Key << "OrthoSize" << YAML::Value << light->OrthoSize;
				out << YAML::Key << "Ambient" << YAML::Value << light->DirLightData.Ambient;
				out << YAML::Key << "Diffuse" << YAML::Value << light->DirLightData.Diffuse;
				out << YAML::Key << "Specular" << YAML::Value << light->DirLightData.Specular;
				break;
			}
			}

			out << YAML::EndMap;
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
			out << YAML::Key << "FreezePositionX" << YAML::Value << rb2d.FreezePositionX;
			out << YAML::Key << "FreezePositionY" << YAML::Value << rb2d.FreezePositionY;
			out << YAML::Key << "AllowSleeping" << YAML::Value << rb2d.AllowSleeping;
			out << YAML::Key << "Awake" << YAML::Value << rb2d.Awake;
			out << YAML::Key << "Bullet" << YAML::Value << rb2d.Bullet;
			out << YAML::Key << "Enabled" << YAML::Value << rb2d.Enabled;
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
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2d.RestitutionThreshold;

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
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2d.RestitutionThreshold;

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
			out << YAML::Key << "EnableLimit" << rjc.EnableLimit;
			out << YAML::Key << "LowerAngle" << rjc.LowerAngle;
			out << YAML::Key << "UpperAngle" << rjc.UpperAngle;
			out << YAML::Key << "MaxMotorTorque" << rjc.MaxMotorTorque;
			out << YAML::Key << "EnableMotor" << rjc.EnableMotor;
			out << YAML::Key << "MotorSpeed" << rjc.MotorSpeed;

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
		OGN_CORE_INFO("[SceneSerializer] Scene Serialized in {0}", filepath);
		PUSH_CONSOLE_INFO("[SceneSerializer] Scene Serialized in {0}", filepath);
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
		OGN_CORE_TRACE("[SceneSerializer] Deserializing scene '{0}'", sceneName);
		PUSH_CONSOLE_INFO("[SceneSerializer] Deserializing scene '{0}'", sceneName);
		if (YAML::Node entities = data["Entities"])
		{
			for (YAML::iterator::value_type entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name = entity["TagComponent"]["Tag"].as<std::string>();
				EntityType type = Utils::EntityTypeStringToType(entity["Type"].as<std::string>());
				Entity deserializedEntity = EntityManager::CreateEntityWithUUID(uuid, name, EntityType::Entity, m_Scene.get());

				deserializedEntity.GetComponent<IDComponent>().Parent = entity["Parent"].as<uint64_t>();

				if (YAML::Node transformComponent = entity["TransformComponent"])
				{
					TransformComponent &tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::quat>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
					tc.Visible = transformComponent["Visible"].as<bool>();
				}

				if (YAML::Node audioListnerComponent = entity["AudioListenerComponent"])
				{
					AudioListenerComponent &al = deserializedEntity.AddComponent<AudioListenerComponent>();
					al.Enable = audioListnerComponent["Enable"].as<bool>();
				}

				if (YAML::Node spriteAnimationComponent = entity["SpriteAnimationComponent"])
				{
					SpriteAnimationComponent &ac = deserializedEntity.AddComponent<SpriteAnimationComponent>();
					if (auto states = spriteAnimationComponent["States"])
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
								frame.Min = frames["Min"].as<glm::vec2>();
								frame.Max = frames["Max"].as<glm::vec2>();
								newAnim->AddFrame(frame);
							}

							ac.State->AddState(name);
							ac.State->AddAnimation(newAnim);
						}
					}
				}

				if (YAML::Node uiComponent = entity["UIComponent"])
				{
					UIComponent &ui = deserializedEntity.AddComponent<UIComponent>();

					for (auto comp : uiComponent["Components"])
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
							component.Component.Min = comp["Min"].as<glm::vec2>();
							component.Component.Max = comp["Max"].as<glm::vec2>();
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

				if (YAML::Node audioComponent = entity["AudioComponent"])
				{
					AudioComponent &ac = deserializedEntity.AddComponent<AudioComponent>();

					if (audioComponent["AudioHandle"])
					{
						ac.Audio = audioComponent["AudioHandle"].as<uint64_t>();
						ac.Name = audioComponent["Name"].as<std::string>();
						ac.Volume = audioComponent["Volume"].as<float>();
						ac.Pitch = audioComponent["Pitch"].as<float>();
						ac.Panning = audioComponent["Panning"].as<float>();
						ac.MinDistance = audioComponent["MinDistance"].as<float>();
						ac.MaxDistance = audioComponent["MaxDistance"].as<float>();
						ac.Looping = audioComponent["Looping"].as<bool>();
						ac.Spatializing = audioComponent["Spatial"].as<bool>();
						ac.PlayAtStart = audioComponent["PlayAtStart"].as<bool>();
						ac.Overlapping = audioComponent["Overlapping"].as<bool>();
						std::shared_ptr<AudioSource> audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
						if (ac.Overlapping)
						{
							audio->ActivateOverlapping();
						}
						audio->SetVolume(ac.Volume);
						audio->SetLoop(ac.Looping);
						audio->SetPitch(ac.Pitch);
						audio->SetMinMaxDistance(ac.MinDistance, ac.MaxDistance);
						audio->SetSpatial(ac.Spatializing);
					}
				}

				if (YAML::Node cameraComponent = entity["CameraComponent"])
				{
					CameraComponent &cc = deserializedEntity.AddComponent<CameraComponent>();
					const auto& cameraProps = cameraComponent["Camera"];
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
					cc.Camera.SetViewportSize(viewportSize.x, viewportSize.y);
					cc.Primary = cameraComponent["Primary"].as<bool>();
				}

				if (YAML::Node staticMeshComponent = entity["StaticMeshComponent"])
				{
					StaticMeshComponent &mc = deserializedEntity.AddComponent<StaticMeshComponent>();
					mc.Name = staticMeshComponent["Name"].as<std::string>();
					mc.HMaterial = staticMeshComponent["HMaterial"].as<uint64_t>();
					mc.HMesh = staticMeshComponent["HMesh"].as<uint64_t>();
					if (mc.HMaterial)
					{
						AssetManager::GetAsset<Material>(mc.HMaterial); // load material and store to memory
					}
					if (mc.HMesh)
					{
						mc.Data = AssetManager::GetAsset<MeshData>(mc.HMesh);
						ModelLoader::ProcessMesh(mc.Data, mc.Data->vertexArray, mc.Data->vertexBuffer);
					}
					mc.mType = static_cast<StaticMeshComponent::Type>(staticMeshComponent["Type"].as<int>());
				}

				if (YAML::Node particleComponent = entity["ParticleComponent"])
				{
					ParticleComponent &pc = deserializedEntity.AddComponent<ParticleComponent>();
					pc.Velocity = particleComponent["Velocity"].as<glm::vec3>();
					pc.VelocityVariation = particleComponent["VelocityVariation"].as<glm::vec3>();
					pc.Rotation = particleComponent["Rotation"].as<glm::vec3>();
					pc.ColorBegin = particleComponent["ColorBegin"].as<glm::vec4>();
					pc.ColorEnd = particleComponent["ColorEnd"].as<glm::vec4>();
					pc.SizeBegin = particleComponent["SizeBegin"].as<float>();
					pc.SizeEnd = particleComponent["SizeEnd"].as<float>();
					pc.SizeVariation = particleComponent["SizeVariation"].as<float>();
					pc.ZAxis = particleComponent["ZAxis"].as<float>();
					pc.LifeTime = particleComponent["LifeTime"].as<float>();
				}

				if (YAML::Node spriteRenderer2DComponent = entity["SpriteRenderer2DComponent"])
				{
					SpriteRenderer2DComponent &src = deserializedEntity.AddComponent<SpriteRenderer2DComponent>();
					src.Color = spriteRenderer2DComponent["Color"].as<glm::vec4>();
					if(spriteRenderer2DComponent["Handle"])
					{
						src.Texture = spriteRenderer2DComponent["Handle"].as<uint64_t>();
						src.Min = spriteRenderer2DComponent["Min"].as<glm::vec2>();
						src.Max = spriteRenderer2DComponent["Max"].as<glm::vec2>();
						src.TillingFactor = spriteRenderer2DComponent["TillingFactor"].as<glm::vec2>();
					}
				}

				if (YAML::Node lightComponent = entity["LightComponent"])
				{
					auto& light = deserializedEntity.AddComponent<LightComponent>().Light;
					//light = Lighting::Create(Utils::LightTypeStringToType(lightComponent["Type"].as<std::string>()));
					
					switch (light->Type)
					{
#if 0
					case LightingType::Spot:
					{
						light->InnerConeAngle = lightComponent["InnerConeAngle"].as<float>();
						light->OuterConeAngle = lightComponent["OuterConeAngle"].as<float>();
						light->Exponent = lightComponent["Exponent"].as<float>();
						break;
					}
					case LightingType::Point:
					{
						light->Ambient = lightComponent["Ambient"].as<float>();
						light->Specular = lightComponent["Specular"].as<float>();
						break;
					}
#endif
					case LightingType::Directional:
					{
						light->DirLightData.Color = lightComponent["Color"].as<glm::vec4>();
						light->DirLightData.Ambient = lightComponent["Ambient"].as<glm::vec4>();
						light->DirLightData.Diffuse = lightComponent["Diffuse"].as<float>();
						light->DirLightData.Specular = lightComponent["Specular"].as<float>();
						light->NearPlane = lightComponent["Near"].as<float>();
						light->FarPlane = lightComponent["Far"].as<float>();
						light->OrthoSize = lightComponent["OrthoSize"].as<float>();
						break;
					}
					}
				}

				if (YAML::Node circleRendererComponent = entity["CircleRendererComponent"])
				{
					CircleRendererComponent &src = deserializedEntity.AddComponent<CircleRendererComponent>();
					src.Color = circleRendererComponent["Color"].as<glm::vec4>();
					src.Fade = circleRendererComponent["Fade"].as<float>();
					src.Thickness = circleRendererComponent["Thickness"].as<float>();
				}

				if (YAML::Node rigidbody2DComponent = entity["Rigidbody2DComponent"])
				{
					Rigidbody2DComponent &rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = Rigidbody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.Mass = rigidbody2DComponent["Mass"].as<float>();
					rb2d.LinearDamping = rigidbody2DComponent["LinearDamping"].as<float>();
					rb2d.AngularDamping = rigidbody2DComponent["AngularDamping"].as<float>();
					rb2d.RotationalInertia = rigidbody2DComponent["RotationalInertia"].as<float>();
					rb2d.GravityScale = rigidbody2DComponent["GravityScale"].as<float>();
					rb2d.MassCenter = rigidbody2DComponent["MassCenter"].as<glm::vec2>();
					rb2d.FreezePositionX = rigidbody2DComponent["FreezePositionX"].as<bool>();
					rb2d.FreezePositionY = rigidbody2DComponent["FreezePositionY"].as<bool>();
					rb2d.AllowSleeping = rigidbody2DComponent["AllowSleeping"].as<bool>();
					rb2d.Awake = rigidbody2DComponent["Awake"].as<bool>();
					rb2d.Bullet = rigidbody2DComponent["Bullet"].as<bool>();
					rb2d.Enabled = rigidbody2DComponent["Enabled"].as<bool>();
					rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				if (YAML::Node boxCollider2DComponent = entity["BoxCollider2DComponent"])
				{
					BoxCollider2DComponent &bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Group = boxCollider2DComponent["Group"].as<int>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				if (YAML::Node circleCollider2DComponent = entity["CircleCollider2DComponent"])
				{
					CircleCollider2DComponent &cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Group = circleCollider2DComponent["Group"].as<int>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				if (YAML::Node revoluteJoint2DComponent = entity["RevoluteJoint2DComponent"])
				{
					RevoluteJoint2DComponent &rjc = deserializedEntity.AddComponent<RevoluteJoint2DComponent>();
					rjc.ConnectedBodyID = revoluteJoint2DComponent["Connected Body ID"].as<uint64_t>();
					rjc.AnchorPoint = revoluteJoint2DComponent["Anchor"].as<glm::vec2>();
					rjc.EnableLimit = revoluteJoint2DComponent["EnableLimit"].as<bool>();
					rjc.LowerAngle = revoluteJoint2DComponent["LowerAngle"].as<float>();
					rjc.UpperAngle = revoluteJoint2DComponent["UpperAngle"].as<float>();
					rjc.MaxMotorTorque = revoluteJoint2DComponent["MaxMotorTorque"].as<float>();
					rjc.EnableMotor = revoluteJoint2DComponent["EnableMotor"].as<bool>();
					rjc.MotorSpeed = revoluteJoint2DComponent["MotorSpeed"].as<float>();
				}

				if (YAML::Node textComponent = entity["TextComponent"])
				{
					AssetHandle handle = textComponent["Handle"].as<uint64_t>();
					if (AssetManager::IsAssetHandleValid(handle))
					{
						TextComponent &tc = deserializedEntity.AddComponent<TextComponent>();
						tc.FontHandle = handle;
						if (handle)
						{
							AssetManager::GetAsset<Font>(handle);
						}
						tc.TextString = textComponent["Text"].as<std::string>();
						tc.Color = textComponent["Color"].as<glm::vec4>();
						tc.Kerning = textComponent["Kerning"].as<float>();
						tc.LineSpacing = textComponent["LineSpacing"].as<float>();
						tc.ScreenSpace = textComponent["ScreenSpace"].as<bool>();
					}
				}

                if (YAML::Node rigidbodyComponent = entity["RigidbodyComponent"])
                {
                    auto &rigidbody = deserializedEntity.AddComponent<RigidbodyComponent>();
                    rigidbody.Mass = rigidbodyComponent["Mass"].as<float>();
                    rigidbody.CenterMass = rigidbodyComponent["CenterMass"].as<glm::vec3>();
					rigidbody.Offset = rigidbodyComponent["Offset"].as<glm::vec3>();
                    rigidbody.UseGravity = rigidbodyComponent["UseGravity"].as<bool>();
                    rigidbody.RotateX = rigidbodyComponent["RotateX"].as<bool>();
                    rigidbody.RotateY = rigidbodyComponent["RotateY"].as<bool>();
                    rigidbody.RotateZ = rigidbodyComponent["RotateZ"].as<bool>();
                    rigidbody.MoveX = rigidbodyComponent["MoveX"].as<bool>();
                    rigidbody.MoveY = rigidbodyComponent["MoveY"].as<bool>();
                    rigidbody.MoveZ = rigidbodyComponent["MoveZ"].as<bool>();
                    rigidbody.IsStatic = rigidbodyComponent["IsStatic"].as<bool>();
                }

                if (YAML::Node boxColliderComponent = entity["BoxColliderComponent"])
                {
                    auto &boxCollider = deserializedEntity.AddComponent<BoxColliderComponent>();
                    boxCollider.Size = boxColliderComponent["Size"].as<glm::vec3>();
                    boxCollider.Restitution = boxColliderComponent["Restitution"].as<float>();
                    boxCollider.Friction = boxColliderComponent["Friction"].as<float>();
                }

                if (YAML::Node sphereColliderComponent = entity["SphereColliderComponent"])
                {
                    auto &sphereCollider = deserializedEntity.AddComponent<SphereColliderComponent>();
                    sphereCollider.Radius = sphereColliderComponent["Radius"].as<float>();
                    sphereCollider.Restitution = sphereColliderComponent["Restitution"].as<float>();
                    sphereCollider.Friction = sphereColliderComponent["Friction"].as<float>();
                }

                if (YAML::Node capsuleColliderComponent = entity["CapsuleColliderComponent"])
                {
                    auto &capsuleCollider = deserializedEntity.AddComponent<CapsuleColliderComponent>();
					capsuleCollider.Radius = capsuleColliderComponent["Radius"].as<float>();
					capsuleCollider.HalfHeight = capsuleColliderComponent["HalfHeight"].as<float>();
                    capsuleCollider.Restitution = capsuleColliderComponent["Restitution"].as<float>();
                    capsuleCollider.Friction = capsuleColliderComponent["Friction"].as<float>();
                }
                
				if (YAML::Node scriptComponent = entity["ScriptComponent"])
				{
					ScriptComponent & sc = deserializedEntity.AddComponent<ScriptComponent>();
					OGN_CORE_ASSERT(deserializedEntity.IsValid(), "SceneSerializer:  Entity is invalid");

					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					if (YAML::Node scriptFields = scriptComponent["ScriptFields"])
					{
						std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClassesByName(sc.ClassName);
						OGN_CORE_ASSERT(entityClass, "SceneSerializer: Entity Class is Invalid");

						if (entityClass)
						{
							const auto& fields = entityClass->GetFields();
							ScriptFieldMap& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

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

}
