// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneSerializer.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Project/Project.h"
#include "Origin/Renderer/Model.h"
#include "Origin/Renderer/Shader.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Audio/Audio.h"
#include "Entity.h"
#include "Lighting.h"
#include "Components.h"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetManager.h"

namespace YAML
{
	template <>
	struct convert<origin::UUID>
	{
		static Node encode(const origin::UUID uuid)
		{
			Node node;
			node.push_back(static_cast<uint64_t>(uuid));
			return node;
		}

		static bool decode(const Node& node, origin::UUID uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};

	template <>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template <>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template <>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

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

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
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
		OGN_CORE_ASSERT(entity.HasComponent<IDComponent>(), "");
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		auto treeComponent = entity.GetComponent<TreeNodeComponent>();
		{
			out << YAML::Key << "TreeNode";
			out << YAML::BeginMap;
			out << YAML::Key << "Parent" << treeComponent.Parent;

			out << YAML::Key << "Parents" << YAML::Value;
			out << YAML::BeginSeq;
			for (auto& p : treeComponent.Parents)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << p.first;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq; //!Parents

			out << YAML::Key << "Children" << YAML::Value;
			out << YAML::BeginSeq;
			for (auto& p : treeComponent.Children)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << p.first;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq; //!Children
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<AnimationComponent>())
		{
			auto& ac = entity.GetComponent<AnimationComponent>();

			out << YAML::Key << "AnimationComponent";
			out << YAML::BeginMap; // AnimationComponent
			out << YAML::Key << "DefaultState" << YAML::Value << ac.State.GetDefaultState();

			out << YAML::Key << "States";
			out << YAML::BeginSeq; // States

			// Find States
			for (int stateIndex = 0; stateIndex < ac.State.GetStateStorage().size(); stateIndex++)
			{
				const std::string currentState = ac.State.GetStateStorage().at(stateIndex);

				out << YAML::BeginMap; // Name
				out << YAML::Key << "Name" << currentState;

				if (ac.State.HasAnimations())
				{
					auto animations = ac.State.GetAnimationState().at(currentState);
					{
						out << YAML::Key << "Frames" << YAML::Value;
						out << YAML::BeginSeq; // Frames
						for (int frameIndex = 0; frameIndex < animations.GetTotalFrames(); frameIndex++)
						{
							out << YAML::BeginMap; // ID
							out << YAML::Key << "ID" << YAML::Value << animations.GetValue(frameIndex); // Add frame path directly to the sequence
							out << YAML::Key << "FrameTime" << YAML::Value << animations.GetAnimationFrames().at(frameIndex).FrameTime;
							out << YAML::EndMap; //!ID
						}
						out << YAML::EndSeq; //!Frames
					}
					out << YAML::Key << "Looping" << YAML::Value << animations.IsLooping();
				}
				out << YAML::EndMap; // !Name
			}
			out << YAML::EndSeq; // !States

			out << YAML::EndMap; // !AnimationComponent
		}


		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent;

			auto& sc = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << sc.ClassName;

			// Fields
			const std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);

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
							OGN_CORE_ERROR("SceneSerializer: {} SCRIPT FIELDS NOT FOUND", name);
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
							WRITE_FIELD_TYPE(Entity, UUID);
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
				out << YAML::Key << "DopplerLevel" << YAML::Value << ac.DopplerLevel;
				out << YAML::Key << "Pitch" << YAML::Value << ac.Pitch;
				out << YAML::Key << "MinDistance" << YAML::Value << ac.MinDistance;
				out << YAML::Key << "MaxDistance" << YAML::Value << ac.MaxDistance;
				out << YAML::Key << "Looping" << YAML::Value << ac.Looping;
				out << YAML::Key << "Spatial" << YAML::Value << ac.Spatial;
				out << YAML::Key << "PlayAtStart" << YAML::Value << ac.PlayAtStart;
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

			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // !Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;

			out << YAML::EndMap; // !CameraComponent
		}

		if (entity.HasComponent<AudioListenerComponent>())
		{
			out << YAML::Key << "AudioListenerComponent";
			out << YAML::BeginMap; // AudioListenerComponent

			const auto& al = entity.GetComponent<AudioListenerComponent>();
			out << YAML::Key << "Enable" << YAML::Value << al.Enable;

			out << YAML::EndMap; // !AudioListenerComponent
		}

		if (entity.HasComponent<StaticMeshComponent>())
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap; // StaticMeshComponent

			const auto& sMesh = entity.GetComponent<StaticMeshComponent>();

			if (sMesh.Model != 0)
			{
				out << YAML::Key << "MeshHandle" << YAML::Value << sMesh.Model;
				std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(sMesh.Model);
				out << YAML::Key << "Color" << YAML::Value << model->GetMaterial()->BufferData.Color;
				out << YAML::Key << "TilingFactor" << YAML::Value << model->GetMaterial()->BufferData.TilingFactor;
				out << YAML::Key << "Metallic" << YAML::Value << model->GetMaterial()->BufferData.TilingFactor;
				out << YAML::Key << "Roughness" << YAML::Value << model->GetMaterial()->BufferData.TilingFactor;
			}
			
			out << YAML::EndMap; // !StaticMeshComponent
		}

		if (entity.HasComponent<BoxColliderComponent>())
		{
			out << YAML::Key << "BoxColliderComponent";
			out << YAML::BeginMap; // BoxColliderComponent
			const auto& bc = entity.GetComponent<BoxColliderComponent>();
			out << YAML::Key << "Size" << YAML::Value << bc.Size;
			out << YAML::Key << "Offset" << YAML::Value << bc.Offset;
			out << YAML::Key << "Restitution" << YAML::Value << bc.Restitution;
			out << YAML::Key << "StaticFriction" << YAML::Value << bc.StaticFriction;
			out << YAML::Key << "DynamicFriction" << YAML::Value << bc.DynamicFriction;

			out << YAML::EndMap; // !BoxColliderComponent
		}

		if (entity.HasComponent<SphereColliderComponent>())
		{
			out << YAML::Key << "SphereColliderComponent";
			out << YAML::BeginMap;
			const auto& sc = entity.GetComponent<SphereColliderComponent>();
			out << YAML::Key << "Radius" << sc.Radius;
			out << YAML::Key << "Offset" << sc.Offset;
			out << YAML::Key << "Restitution" << sc.Restitution;
			out << YAML::Key << "StaticFriction" << YAML::Value << sc.StaticFriction;
			out << YAML::Key << "DynamicFriction" << YAML::Value << sc.DynamicFriction;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CapsuleColliderComponent>())
		{
			out << YAML::Key << "CapsuleColliderComponent";
			out << YAML::BeginMap; // CapsuleColliderComponent
			const auto& cc = entity.GetComponent<CapsuleColliderComponent>();
			out << YAML::Key << "Radius" << YAML::Value << cc.Radius;
			out << YAML::Key << "Height" << YAML::Value << cc.Height;
			out << YAML::Key << "Offset" << YAML::Value << cc.Offset;
			out << YAML::Key << "Restitution" << YAML::Value << cc.Restitution;
			out << YAML::Key << "StaticFriction" << YAML::Value << cc.StaticFriction;
			out << YAML::Key << "DynamicFriction" << YAML::Value << cc.DynamicFriction;

			out << YAML::EndMap; // !CapsuleColliderComponent
		}

		if (entity.HasComponent<RigidbodyComponent>())
		{
			out << YAML::Key << "RigidbodyComponent";
			out << YAML::BeginMap; // RigidbodyComponent
			const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

			out << YAML::Key << "Mass" << YAML::Value << rigidbody.Mass;
			out << YAML::Key << "CenterMassPosition" << YAML::Value << rigidbody.CenterMassPosition;

			out << YAML::Key << "UseGravity" << YAML::Value << rigidbody.UseGravity;
			out << YAML::Key << "RotateX" << YAML::Value << rigidbody.RotateX;
			out << YAML::Key << "RotateY" << YAML::Value << rigidbody.RotateY;
			out << YAML::Key << "RotateZ" << YAML::Value << rigidbody.RotateZ;
			out << YAML::Key << "MoveX" << YAML::Value << rigidbody.MoveX;
			out << YAML::Key << "MoveY" << YAML::Value << rigidbody.MoveY;
			out << YAML::Key << "MoveZ" << YAML::Value << rigidbody.MoveZ;
			out << YAML::Key << "Kinematic" << YAML::Value << rigidbody.Kinematic;
			out << YAML::Key << "RetainAcceleration" << YAML::Value << rigidbody.RetainAcceleration;

			out << YAML::EndMap; // !Rigidbody
		}

		if (entity.HasComponent<TextComponent>())
		{
			out << YAML::Key << "TextComponent";
			out << YAML::BeginMap; // TextComponent
			const auto& textComponent = entity.GetComponent<TextComponent>();
			out << YAML::Key << "TextString" << YAML::Value << textComponent.TextString;
			const auto& textAssetPath = relative(textComponent.FontAsset->GetFilepath(), Project::GetActiveAssetDirectory());
			out << YAML::Key << "FontFilepath" << YAML::Value << textAssetPath.generic_string();
			out << YAML::Key << "Color" << YAML::Value << textComponent.Color;
			out << YAML::Key << "Kerning" << YAML::Value << textComponent.Kerning;
			out << YAML::Key << "LineSpacing" << YAML::Value << textComponent.LineSpacing;

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

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			const auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << src.Color;
			if (src.Texture)
			{
				auto& texturePath = relative(src.Texture->GetFilepath(), Project::GetActiveAssetDirectory());
				out << YAML::Key << "TexturePath" << YAML::Value << texturePath.generic_string();
			}

			out << YAML::EndMap; // !SpriteRendererComponent
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
				out << YAML::Key << "TillingFactor" << YAML::Value << src.TillingFactor;
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
				out << YAML::Key << "Color" << YAML::Value << light->m_DirLightData.Color;
				out << YAML::Key << "Strength" << YAML::Value << light->m_DirLightData.Strength;
				out << YAML::Key << "Diffuse" << YAML::Value << light->m_DirLightData.Diffuse;
				out << YAML::Key << "Specular" << YAML::Value << light->m_DirLightData.Specular;
				break;
			}
			default:
				break;
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

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filepath.filename().string();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = {entityID, m_Scene.get()};
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath.string());
		fout << out.c_str();
		fout.close();

		OGN_CORE_INFO("Scene Serialized in {0}", filepath.string());
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& filepath)
	{
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath.string());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		auto sceneName = data["Scene"].as<std::string>();
		OGN_CORE_TRACE("Deserializing scene '{0}'", sceneName);
		if (YAML::Node entities = data["Entities"])
		{
			for (YAML::iterator::value_type entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name;
				if (auto tagComponent = entity["TagComponent"])
					name = tagComponent["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				if (YAML::Node transformComponent = entity["TransformComponent"])
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				if (YAML::Node audioListnerComponent = entity["AudioListenerComponent"])
				{
					auto& al = deserializedEntity.AddComponent<AudioListenerComponent>();
					al.Enable = audioListnerComponent["Enable"].as<bool>();
				}

				if (YAML::Node animationComponent = entity["AnimationComponent"])
				{
					auto& ac = deserializedEntity.AddComponent<AnimationComponent>();

					if (auto states = animationComponent["States"])
					{
						// Get all states
						for (auto state : states)
						{
							Animation animation;
							ac.State.AddState(state["Name"].as<std::string>());
							
							// Retrieve all frames from the state
							for (auto frames : state["Frames"])
							{
								AssetHandle handle = frames["ID"].as<uint64_t>();
								animation.AddFrame(handle, frames["FrameTime"].as<float>());
							}

							// Add the animation after frames added
							ac.State.AddAnimation(animation);
							ac.State.SetLooping(state["Name"].as<std::string>(), state["Looping"].as<bool>());
						}
					}

					ac.State.SetActiveState(animationComponent["DefaultState"].as<std::string>());
				}

				if (YAML::Node audioComponent = entity["AudioComponent"])
				{
					auto& ac = deserializedEntity.AddComponent<AudioComponent>();

					if (audioComponent["AudioHandle"])
					{
						ac.Audio = audioComponent["AudioHandle"].as<uint64_t>();
						ac.Name = audioComponent["Name"].as<std::string>();
						ac.Volume = audioComponent["Volume"].as<float>();
						ac.DopplerLevel = audioComponent["DopplerLevel"].as<float>();
						ac.Pitch = audioComponent["Pitch"].as<float>();
						ac.MinDistance = audioComponent["MinDistance"].as<float>();
						ac.MaxDistance = audioComponent["MaxDistance"].as<float>();
						ac.Looping = audioComponent["Looping"].as<bool>();
						ac.Spatial = audioComponent["Spatial"].as<bool>();
						ac.PlayAtStart = audioComponent["PlayAtStart"].as<bool>();

						auto& audio = AssetManager::GetAsset<Audio>(ac.Audio);

						audio->SetGain(ac.Volume);
						audio->SetDopplerLevel(ac.DopplerLevel);
						audio->SetLoop(ac.Looping);
						audio->SetPitch(ac.Pitch);
						audio->SetMinDistance(ac.MinDistance);
						audio->SetMaxDistance(ac.MaxDistance);
						audio->SetSpatial(ac.Spatial);
					}
				}

				if (YAML::Node cameraComponent = entity["CameraComponent"])
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType(static_cast<ProjectionType>(cameraProps["ProjectionType"].as<int>()));
					cc.Camera.SetAspectRatioType(static_cast<SceneCamera::AspectRatioType>(cameraProps["AspectRatioType"].as<int>()));

					cc.Camera.SetPerspectiveFov(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
				}

				if (YAML::Node spriteRendererComponent = entity["SpriteRendererComponent"])
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["TexturePath"])
						src.Texture = Texture2D::Create(spriteRendererComponent["TexturePath"].as<std::string>());
				}

				if (YAML::Node particleComponent = entity["ParticleComponent"])
				{
					auto& pc = deserializedEntity.AddComponent<ParticleComponent>();
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
					auto& src = deserializedEntity.AddComponent<SpriteRenderer2DComponent>();
					src.Color = spriteRenderer2DComponent["Color"].as<glm::vec4>();
					if(spriteRenderer2DComponent["Handle"])
					{
						src.Texture = spriteRenderer2DComponent["Handle"].as<uint64_t>();
						src.TillingFactor = spriteRenderer2DComponent["TillingFactor"].as<glm::vec2>();
					}
				}

				if (YAML::Node lightComponent = entity["LightComponent"])
				{
					auto& light = deserializedEntity.AddComponent<LightComponent>().Light;
					light = Lighting::Create(Utils::LightTypeStringToType(lightComponent["Type"].as<std::string>()));
					
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
						light->m_DirLightData.Color = lightComponent["Color"].as<glm::vec4>();
						light->m_DirLightData.Strength = lightComponent["Strength"].as<float>();
						light->m_DirLightData.Diffuse = lightComponent["Diffuse"].as<float>();
						light->m_DirLightData.Specular = lightComponent["Specular"].as<float>();
						break;
					}
					}
				}

				if (YAML::Node circleRendererComponent = entity["CircleRendererComponent"])
				{
					auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
					src.Color = circleRendererComponent["Color"].as<glm::vec4>();
					src.Fade = circleRendererComponent["Fade"].as<float>();
					src.Thickness = circleRendererComponent["Thickness"].as<float>();
				}

				if (YAML::Node rigidbody2DComponent = entity["Rigidbody2DComponent"])
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
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
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
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
					auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Group = circleCollider2DComponent["Group"].as<int>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				if (YAML::Node staticMeshComponent = entity["StaticMeshComponent"])
				{
					auto& sMesh = deserializedEntity.AddComponent<StaticMeshComponent>();

					if (staticMeshComponent["MeshHandle"])
					{
						sMesh.Model = staticMeshComponent["MeshHandle"].as<uint64_t>();
						std::shared_ptr<Model>& model = AssetManager::GetAsset<Model>(sMesh.Model);
						model->GetMaterial()->BufferData.Color = staticMeshComponent["Color"].as<glm::vec4>();
						model->GetMaterial()->BufferData.TilingFactor = staticMeshComponent["TilingFactor"].as<glm::vec2>();
						//model->GetMaterial()->BufferData.Metallic = staticMeshComponent["Metallic"].as<float>();
						//model->GetMaterial()->BufferData.Roughness = staticMeshComponent["Roughness"].as<float>();
					}
					
				}

				if (YAML::Node boxColliderComponent = entity["BoxColliderComponent"])
				{
					auto& boxCollider = deserializedEntity.AddComponent<BoxColliderComponent>();
					boxCollider.Size = boxColliderComponent["Size"].as<glm::vec3>();
					boxCollider.Offset = boxColliderComponent["Offset"].as<glm::vec3>();
					boxCollider.Restitution = boxColliderComponent["Restitution"].as<float>();
					boxCollider.StaticFriction = boxColliderComponent["StaticFriction"].as<float>();
					boxCollider.DynamicFriction = boxColliderComponent["DynamicFriction"].as<float>();
				}

				if (YAML::Node sphereColliderComponent = entity["SphereColliderComponent"])
				{
					auto& sphereCollider = deserializedEntity.AddComponent<SphereColliderComponent>();
					sphereCollider.Radius = sphereColliderComponent["Radius"].as<float>();
					sphereCollider.Offset = sphereColliderComponent["Offset"].as<glm::vec3>();
					sphereCollider.Restitution = sphereColliderComponent["Restitution"].as<float>();
					sphereCollider.StaticFriction = sphereColliderComponent["StaticFriction"].as<float>();
					sphereCollider.DynamicFriction = sphereColliderComponent["DynamicFriction"].as<float>();
				}

				if (YAML::Node capsueColliderComponent = entity["CapsuleColliderComponent"])
				{
					auto& capsuleCollider = deserializedEntity.AddComponent<CapsuleColliderComponent>();
					capsuleCollider.Radius = capsueColliderComponent["Radius"].as<float>();
					capsuleCollider.Height = capsueColliderComponent["Height"].as<float>();
					capsuleCollider.Offset = capsueColliderComponent["Offset"].as<glm::vec3>();
					capsuleCollider.Restitution = capsueColliderComponent["Restitution"].as<float>();
					capsuleCollider.StaticFriction = capsueColliderComponent["StaticFriction"].as<float>();
					capsuleCollider.DynamicFriction = capsueColliderComponent["DynamicFriction"].as<float>();
				}

				if (YAML::Node rigidbodyComponent = entity["RigidbodyComponent"])
				{
					auto& rigidbody = deserializedEntity.AddComponent<RigidbodyComponent>();
					rigidbody.Mass = rigidbodyComponent["Mass"].as<float>();
					rigidbody.CenterMassPosition = rigidbodyComponent["CenterMassPosition"].as<glm::vec3>();
					rigidbody.UseGravity = rigidbodyComponent["UseGravity"].as<bool>();
					rigidbody.RotateX = rigidbodyComponent["RotateX"].as<bool>();
					rigidbody.RotateY = rigidbodyComponent["RotateY"].as<bool>();
					rigidbody.RotateZ = rigidbodyComponent["RotateZ"].as<bool>();
					rigidbody.MoveX = rigidbodyComponent["MoveX"].as<bool>();
					rigidbody.MoveY = rigidbodyComponent["MoveY"].as<bool>();
					rigidbody.MoveZ = rigidbodyComponent["MoveZ"].as<bool>();
					rigidbody.Kinematic = rigidbodyComponent["Kinematic"].as<bool>();
					rigidbody.RetainAcceleration = rigidbodyComponent["RetainAcceleration"].as<bool>();

				}

				if (YAML::Node revoluteJoint2DComponent = entity["RevoluteJoint2DComponent"])
				{
					auto& rjc = deserializedEntity.AddComponent<RevoluteJoint2DComponent>();
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
					auto& text = deserializedEntity.AddComponent<TextComponent>();
					auto filepath = textComponent["FontFilepath"].as<std::string>();

					if (filepath.empty() || filepath == Font::GetDefault()->GetFilepath())
						text.FontAsset = Font::GetDefault();

					else if (!filepath.empty() && !text.FontAsset)
					{
						auto path = Project::GetActiveAssetFileSystemPath(filepath);
						text.FontAsset = std::make_shared<Font>(path);
					}

					text.TextString = textComponent["TextString"].as<std::string>();
					text.Color = textComponent["Color"].as<glm::vec4>();
					text.Kerning = textComponent["Kerning"].as<float>();
					text.LineSpacing = textComponent["LineSpacing"].as<float>();
				}

				if (YAML::Node scriptComponent = entity["ScriptComponent"])
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					OGN_CORE_ASSERT(deserializedEntity, "Entity is invalid");

					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					if (auto scriptFields = scriptComponent["ScriptFields"])
					{
						std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
						OGN_CORE_ASSERT(entityClass, "Entity Class is Invalid");

						if (entityClass)
						{
							const auto& fields = entityClass->GetFields();
							auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

							for (auto scriptField : scriptFields)
							{
								auto name = scriptField["Name"].as<std::string>();
								auto typeString = scriptField["Type"].as<std::string>();
								ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

								ScriptFieldInstance& fieldInstance = entityFields[name];

								// for Log in editor
								OGN_CORE_ASSERT(fields.find(name) != fields.end(), "Script Fields Not Found");
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
									READ_FIELD_TYPE(Entity, UUID);
								}
							}
						}
					}
				}
			}

			// Tree Node Deserialize
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				Entity newEntity = m_Scene->GetEntityWithUUID(uuid);
				auto& treeComponent = newEntity.GetComponent<TreeNodeComponent>();

				if (YAML::Node treeNode = entity["TreeNode"])
				{
					treeComponent.Parent = treeNode["Parent"].as<uint64_t>();
					if (auto parents = treeNode["Parents"])
					{
						for (auto p : parents)
						{
							UUID uuid = p["ID"].as<uint64_t>();
							Entity e = m_Scene->GetEntityWithUUID(uuid);
							treeComponent.Parents[e.GetUUID()] = e;
						}
					}

					if (auto children = treeNode["Children"])
					{
						for (auto c : children)
						{
							UUID uuid = c["ID"].as<uint64_t>();
							Entity e = m_Scene->GetEntityWithUUID(uuid);
							treeComponent.Children[e.GetUUID()] = e;
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
}
