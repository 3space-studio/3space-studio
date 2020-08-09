#ifndef DARKSTARDTSCONVERTER_STRUCTURES_HPP
#define DARKSTARDTSCONVERTER_STRUCTURES_HPP

#include <variant>
#include <array>
#include <string>
#include <vector>
#include "endian_arithmetic.hpp"

namespace darkstar::dts
{
  namespace endian = boost::endian;
  using file_tag = std::array<std::byte, 4>;

  template<std::size_t Size>
  constexpr std::array<std::string_view, Size> make_keys(const char*(&&keys)[Size])
  {
    std::array<std::string_view, Size> result;
    for (auto i = 0; i < Size; i++)
    {
      result[i] = keys[i];
    }
    return result;
  }

  constexpr file_tag to_tag(const std::array<std::uint8_t, 4> values)
  {
    file_tag result{};

    for (auto i = 0u; i < values.size(); i++)
    {
      result[i] = std::byte{ values[i] };
    }
    return result;
  }

  constexpr file_tag pers_tag = to_tag({ 'P', 'E', 'R', 'S' });

  using version = endian::little_uint32_t;

  struct file_info
  {
    endian::little_int32_t file_length;
    endian::little_int16_t class_name_length;
  };

  struct tag_header
  {
    dts::file_tag tag;
    dts::file_info file_info;
    std::string class_name;
    dts::version version;
  };

  struct vector3f
  {
    constexpr static auto keys = make_keys({ "x", "y", "z" });
    float x;
    float y;
    float z;
  };

  struct vector3f_pair
  {
    constexpr static auto keys = make_keys({ "min", "max" });
    vector3f min;
    vector3f max;
  };

  static_assert(sizeof(vector3f) == sizeof(std::array<float, 3>));

  struct quaternion4s
  {
    constexpr static auto keys = make_keys({ "x", "y", "z", "w" });
    endian::little_int16_t x;
    endian::little_int16_t y;
    endian::little_int16_t z;
    endian::little_int16_t w;
  };

  struct quaternion4f
  {
    constexpr static auto keys = make_keys({ "x", "y", "z", "w" });
    float x;
    float y;
    float z;
    float w;
  };

  static_assert(sizeof(quaternion4s) == sizeof(std::array<endian::little_int16_t, 4>));

  struct rgb_data
  {
    constexpr static auto keys = make_keys({ "red", "green", "blue", "rgbFlags" });

    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t rgb_flags;
  };


  namespace mesh::v1
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numVerts",
                                               "vertsPerFrame",
                                               "numTextureVerts",
                                               "numFaces",
                                               "numFrames",
                                               "scale",
                                               "origin",
                                               "radius" });

      endian::little_int32_t num_verts;
      endian::little_int32_t verts_per_frame;
      endian::little_int32_t num_texture_verts;
      endian::little_int32_t num_faces;
      endian::little_int32_t num_frames;
      vector3f scale;
      vector3f origin;
      float radius;
    };

    struct vertex
    {
      constexpr static auto keys = make_keys({ "x", "y", "z", "normal" });
      std::uint8_t x;
      std::uint8_t y;
      std::uint8_t z;
      std::uint8_t normal;
    };

    static_assert(sizeof(vertex) == sizeof(std::int32_t));

    struct texture_vertex
    {
      constexpr static auto keys = make_keys({ "x", "y" });
      float x;
      float y;
    };

    struct face
    {
      constexpr static auto keys = make_keys({ "vi1", "ti1", "vi2", "ti2", "vi3", "ti3", "material" });
      endian::little_int32_t vi1;
      endian::little_int32_t ti1;
      endian::little_int32_t vi2;
      endian::little_int32_t ti2;
      endian::little_int32_t vi3;
      endian::little_int32_t ti3;
      endian::little_int32_t material;
    };

    struct frame
    {
      constexpr static auto keys = make_keys({ "firstVert" });
      endian::little_int32_t first_vert;
    };

    struct mesh
    {
      constexpr static auto type_name = std::string_view{ "TS::CelAnimMesh" };
      constexpr static auto version = 1;
      constexpr static auto keys = make_keys({ "header", "vertices", "textureVertices", "faces", "frames" });

      header header;
      std::vector<vertex> vertices;
      std::vector<texture_vertex> texture_vertices;
      std::vector<face> faces;
      std::vector<frame> frames;
    };
  }// namespace mesh::v1

  namespace mesh::v2
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numVerts",
                                               "vertsPerFrame",
                                               "numTextureVerts",
                                               "numFaces",
                                               "numFrames",
                                               "textureVertsPerFrame",
                                               "scale",
                                               "origin",
                                               "radius" });

      endian::little_int32_t num_verts;
      endian::little_int32_t verts_per_frame;
      endian::little_int32_t num_texture_verts;
      endian::little_int32_t num_faces;
      endian::little_int32_t num_frames;
      endian::little_int32_t texture_verts_per_frame;
      vector3f scale;
      vector3f origin;
      float radius;
    };

    struct mesh
    {
      constexpr static auto type_name = v1::mesh::type_name;
      constexpr static auto version = 2;
      constexpr static auto keys = make_keys({ "header", "vertices", "textureVertices", "faces", "frames" });

      header header;
      std::vector<v1::vertex> vertices;
      std::vector<v1::texture_vertex> texture_vertices;
      std::vector<v1::face> faces;
      std::vector<v1::frame> frames;
    };
  }// namespace mesh::v2

  namespace mesh::v3
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numVerts",
                                               "vertsPerFrame",
                                               "numTextureVerts",
                                               "numFaces",
                                               "numFrames",
                                               "textureVertsPerFrame",
                                               "radius" });

      endian::little_int32_t num_verts;
      endian::little_int32_t verts_per_frame;
      endian::little_int32_t num_texture_verts;
      endian::little_int32_t num_faces;
      endian::little_int32_t num_frames;
      endian::little_int32_t texture_verts_per_frame;
      float radius;
    };

    struct frame
    {
      constexpr static auto keys = make_keys({ "firstVert", "scale", "origin" });
      endian::little_int32_t first_vert;
      vector3f scale;
      vector3f origin;
    };

    struct mesh
    {
      constexpr static auto type_name = v1::mesh::type_name;
      constexpr static auto version = 3;
      constexpr static auto keys = make_keys({ "header", "vertices", "textureVertices", "faces", "frames" });

      v3::header header;
      std::vector<v1::vertex> vertices;
      std::vector<v1::texture_vertex> texture_vertices;
      std::vector<v1::face> faces;
      std::vector<v3::frame> frames;
    };

  }// namespace mesh::v3

  namespace material_list::v2
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numDetails",
                                               "numMaterials" });
      endian::little_int32_t num_details;
      endian::little_int32_t num_materials;
    };

    struct material
    {
      constexpr static auto keys = make_keys({ "flags", "alpha", "index", "rgbData", "fileName" });

      endian::little_int32_t flags;
      float alpha;
      endian::little_int32_t index;
      rgb_data rgb_data;

      std::array<char, 32> file_name;
    };

    struct material_list
    {
      constexpr static auto type_name = std::string_view{ "TS::MaterialList" };
      constexpr static auto version = 2;
      constexpr static auto keys = make_keys({ "header", "materials" });

      header header;
      std::vector<material> materials;
    };
  }// namespace material_list::v2

  namespace material_list::v3
  {
    struct material
    {
      constexpr static auto keys = make_keys({ "flags", "alpha", "index", "rgbData", "fileName", "type", "elasticity", "friction" });

      endian::little_int32_t flags;
      float alpha;
      endian::little_int32_t index;
      rgb_data rgb_data;

      std::array<char, 32> file_name;

      endian::little_int32_t type;
      float elasticity;
      float friction;
    };

    struct material_list
    {
      constexpr static auto type_name = v2::material_list::type_name;
      constexpr static auto version = 3;
      constexpr static auto keys = make_keys({ "header", "materials" });

      v2::header header;
      std::vector<material> materials;
    };
  }// namespace material_list::v3

  namespace material_list::v4
  {
    struct material
    {
      constexpr static auto keys = make_keys({ "flags", "alpha", "index", "rgbData", "fileName", "type", "elasticity", "friction", "useDefaultProperties" });

      endian::little_int32_t flags;
      float alpha;
      endian::little_int32_t index;
      rgb_data rgb_data;

      std::array<char, 32> file_name;

      endian::little_int32_t type;
      float elasticity;
      float friction;
      endian::little_uint32_t use_default_properties;
    };

    struct material_list
    {
      constexpr static auto type_name = v2::material_list::type_name;
      constexpr static auto version = 4;
      constexpr static auto keys = make_keys({ "header", "materials" });

      v2::header header;
      std::vector<material> materials;
    };

  }// namespace material_list::v4

  using mesh_variant = std::variant<mesh::v1::mesh, mesh::v2::mesh, mesh::v3::mesh>;

  using material_list_variant = std::variant<material_list::v2::material_list, material_list::v3::material_list, material_list::v4::material_list>;

  namespace shape::v2
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numNodes",
        "numSequences",
        "numSubSequences",
        "numKeyFrames",
        "numTransforms",
        "numNames",
        "numObjects",
        "numDetails",
        "numMeshes",
        "numTransitions" });

      endian::little_int32_t num_nodes;
      endian::little_int32_t num_sequences;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t num_key_frames;
      endian::little_int32_t num_transforms;
      endian::little_int32_t num_names;
      endian::little_int32_t num_objects;
      endian::little_int32_t num_details;
      endian::little_int32_t num_meshes;
      endian::little_int32_t num_transitions;
    };

    struct data
    {
      constexpr static auto keys = make_keys({ "radius", "centre" });
      float radius;
      vector3f centre;
    };

    static_assert(sizeof(data) == sizeof(std::array<float, 4>));

    struct node
    {
      constexpr static auto keys = make_keys({ "name", "parent", "numSubSequences", "firstSubSequence", "defaultTransform" });
      endian::little_int32_t name;
      endian::little_int32_t parent;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t first_sub_sequence;
      endian::little_int32_t default_transform;
    };

    struct sequence
    {
      constexpr static auto keys = make_keys({ "nameIndex",
                                               "cyclic",
                                               "duration",
                                               "priority" });
      endian::little_int32_t name_index;
      endian::little_int32_t cyclic;
      float duration;
      endian::little_int32_t priority;
    };

    struct sub_sequence
    {
      constexpr static auto keys = make_keys({ "sequenceIndex", "numKeyFrames", "firstKeyFrame" });
      endian::little_int32_t sequence_index;
      endian::little_int32_t num_key_frames;
      endian::little_int32_t first_key_frame;
    };

    struct keyframe
    {
      constexpr static auto keys = make_keys({ "position", "keyValue" });
      float position;
      endian::little_uint32_t key_value;
    };

    struct transform
    {
      constexpr static auto keys = make_keys({ "rotation", "translation", "scale" });
      quaternion4f rotation;
      vector3f translation;
      vector3f scale;
    };

    using name = std::array<char, 24>;

    struct object
    {
      constexpr static auto keys = make_keys({ "nameIndex", "flags", "meshIndex", "nodeIndex", "depFlags", "dep", "objectOffset", "numSubSequences", "firstSubSequence" });
      endian::little_int16_t name_index;
      endian::little_int16_t flags;
      endian::little_int32_t mesh_index;
      endian::little_int32_t node_index;
      endian::little_int32_t dep_flags;
      std::array<vector3f, 3> dep;
      vector3f object_offset;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t first_sub_sequence;
    };

    struct detail
    {
      constexpr static auto keys = make_keys({ "nameIndex", "size" });
      endian::little_int32_t name_index;
      float size;
    };

    struct transition
    {
      constexpr static auto keys = make_keys({ "startSequence",
                                               "endSequence",
                                               "startPosition",
                                               "endPosition",
                                               "duration",
                                               "transform" });
      endian::little_int32_t start_sequence;
      endian::little_int32_t end_sequence;
      float start_position;
      float end_position;
      float duration;
      transform transform;
    };

    // TODO Put this into the actual shape for 100% coverage in the JSON file.
    using has_material_list_flag = endian::little_int32_t;

    struct shape
    {
      constexpr static auto type_name = std::string_view{ "TS::Shape" };
      constexpr static auto version = 2;
      constexpr static auto keys = make_keys({ "header",
                                               "data",
                                               "nodes",
                                               "sequences",
                                               "subSequences",
                                               "keyframes",
                                               "transforms",
                                               "names",
                                               "objects",
                                               "details",
                                               "transitions",
                                               "meshes",
                                               "materialList" });

      header header;
      data data;
      std::vector<node> nodes;
      std::vector<sequence> sequences;
      std::vector<sub_sequence> sub_sequences;
      std::vector<keyframe> keyframes;
      std::vector<transform> transforms;
      std::vector<name> names;
      std::vector<object> objects;
      std::vector<detail> details;
      std::vector<transition> transitions;
      std::vector<mesh_variant> meshes;

      material_list_variant material_list;
    };
  }// namespace shape::v2

  namespace shape::v3
  {
    struct keyframe
    {
      constexpr static auto keys = make_keys({ "position", "keyValue", "matIndex" });
      float position;
      endian::little_uint32_t key_value;
      endian::little_uint32_t mat_index;
    };

    struct shape
    {
      constexpr static auto type_name = v2::shape::type_name;
      constexpr static auto version = 3;
      constexpr static auto keys = make_keys({ "header",
                                               "data",
                                               "nodes",
                                               "sequences",
                                               "subSequences",
                                               "keyframes",
                                               "transforms",
                                               "names",
                                               "objects",
                                               "details",
                                               "transitions",
                                               "meshes",
                                               "materialList" });

      v2::header header;
      v2::data data;
      std::vector<v2::node> nodes;
      std::vector<v2::sequence> sequences;
      std::vector<v2::sub_sequence> sub_sequences;
      std::vector<keyframe> keyframes;
      std::vector<v2::transform> transforms;
      std::vector<v2::name> names;
      std::vector<v2::object> objects;
      std::vector<v2::detail> details;
      std::vector<v2::transition> transitions;
      std::vector<mesh_variant> meshes;

      material_list_variant material_list;
    };
  }

  namespace shape::v5
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numNodes",
                                               "numSequences",
                                               "numSubSequences",
                                               "numKeyFrames",
                                               "numTransforms",
                                               "numNames",
                                               "numObjects",
                                               "numDetails",
                                               "numMeshes",
                                               "numTransitions",
                                               "numFrameTriggers" });

      endian::little_int32_t num_nodes;
      endian::little_int32_t num_sequences;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t num_key_frames;
      endian::little_int32_t num_transforms;
      endian::little_int32_t num_names;
      endian::little_int32_t num_objects;
      endian::little_int32_t num_details;
      endian::little_int32_t num_meshes;
      endian::little_int32_t num_transitions;
      endian::little_int32_t num_frame_triggers;
    };

    struct sequence
    {
      constexpr static auto keys = make_keys({ "nameIndex",
                                               "cyclic",
                                               "duration",
                                               "priority",
                                               "firstFrameTrigger",
                                               "numFrameTriggers",
                                               "numIflSubSequences",
                                               "firstIflSubSequence" });
      endian::little_int32_t name_index;
      endian::little_int32_t cyclic;
      float duration;
      endian::little_int32_t priority;
      endian::little_int32_t first_frame_trigger;
      endian::little_int32_t num_frame_triggers;
      endian::little_int32_t num_ifl_sub_sequences;
      endian::little_int32_t first_ifl_sub_sequence;
    };

    struct frame_trigger
    {
      constexpr static auto keys = make_keys({ "position",
                                               "value" });
      float position;
      float value;
    };

    struct footer
    {
      constexpr static auto keys = make_keys({ "numDefaultMaterials" });
      endian::little_int32_t num_default_materials;
    };

    struct shape
    {
      constexpr static auto type_name = v2::shape::type_name;
      constexpr static auto version = 5;
      constexpr static auto keys = make_keys({ "header",
                                               "data",
                                               "nodes",
                                               "sequences",
                                               "subSequences",
                                               "keyframes",
                                               "transforms",
                                               "names",
                                               "objects",
                                               "details",
                                               "transitions",
                                               "frameTriggers",
                                               "footer",
                                               "meshes",
                                               "materialList" });

      header header;
      v2::data data;
      std::vector<v2::node> nodes;
      std::vector<sequence> sequences;
      std::vector<v2::sub_sequence> sub_sequences;
      std::vector<v3::keyframe> keyframes;
      std::vector<v2::transform> transforms;
      std::vector<v2::name> names;
      std::vector<v2::object> objects;
      std::vector<v2::detail> details;
      std::vector<v2::transition> transitions;
      std::vector<frame_trigger> frame_triggers;
      v5::footer footer;
      std::vector<mesh_variant> meshes;

      material_list_variant material_list;
    };
  }// namespace shape::v5


  namespace shape::v6
  {
    struct footer
    {
      constexpr static auto keys = make_keys({ "numDefaultMaterials",
                                               "alwaysNode" });
      endian::little_int32_t num_default_materials;
      endian::little_int32_t always_node;
    };

    struct shape
    {
      constexpr static auto type_name = v2::shape::type_name;
      constexpr static auto version = 6;
      constexpr static auto keys = make_keys({ "header",
                                               "data",
                                               "nodes",
                                               "sequences",
                                               "subSequences",
                                               "keyframes",
                                               "transforms",
                                               "names",
                                               "objects",
                                               "details",
                                               "transitions",
                                               "frameTriggers",
                                               "footer",
                                               "meshes",
                                               "materialList" });

      v5::header header;
      v2::data data;
      std::vector<v2::node> nodes;
      std::vector<v5::sequence> sequences;
      std::vector<v2::sub_sequence> sub_sequences;
      std::vector<v3::keyframe> keyframes;
      std::vector<v2::transform> transforms;
      std::vector<v2::name> names;
      std::vector<v2::object> objects;
      std::vector<v2::detail> details;
      std::vector<v2::transition> transitions;
      std::vector<v5::frame_trigger> frame_triggers;
      footer footer;
      std::vector<mesh_variant> meshes;

      material_list_variant material_list;
    };
  }// namespace shape::v6

  namespace shape::v7
  {
    struct transform
    {
      constexpr static auto keys = make_keys({ "rotation", "translation", "scale" });
      quaternion4s rotation;
      vector3f translation;
      vector3f scale;
    };

    static_assert(sizeof(transform) == sizeof(std::array<std::int32_t, 8>));

    struct transition
    {
      constexpr static auto keys = make_keys({ "startSequence",
        "endSequence",
        "startPosition",
        "endPosition",
        "duration",
        "rotation",
        "translation",
        "scale" });
      endian::little_int32_t start_sequence;
      endian::little_int32_t end_sequence;
      float start_position;
      float end_position;
      float duration;
      quaternion4s rotation;
      vector3f translation;
      vector3f scale;
    };

    struct shape
    {
      constexpr static auto type_name = v2::shape::type_name;
      constexpr static auto version = 7;
      constexpr static auto keys = make_keys({ "header",
                                               "data",
                                               "nodes",
                                               "sequences",
                                               "subSequences",
                                               "keyframes",
                                               "transforms",
                                               "names",
                                               "objects",
                                               "details",
                                               "transitions",
                                               "frameTriggers",
                                               "footer",
                                               "meshes",
                                               "materialList" });

      v5::header header;
      v2::data data;
      std::vector<v2::node> nodes;
      std::vector<v5::sequence> sequences;
      std::vector<v2::sub_sequence> sub_sequences;
      std::vector<v3::keyframe> keyframes;
      std::vector<transform> transforms;
      std::vector<v2::name> names;
      std::vector<v2::object> objects;
      std::vector<v2::detail> details;
      std::vector<transition> transitions;
      std::vector<v5::frame_trigger> frame_triggers;
      v6::footer footer;
      std::vector<mesh_variant> meshes;

      material_list_variant material_list;
    };
  }// namespace shape::v7

  namespace shape::v8
  {
    struct data
    {
      constexpr static auto keys = make_keys({ "radius", "centre", "bounds" });
      float radius;
      vector3f centre;
      vector3f_pair bounds;
    };

    static_assert(sizeof(data) == sizeof(std::array<float, 10>));

    struct node
    {
      constexpr static auto keys = make_keys({ "name", "parent", "numSubSequences", "firstSubSequence", "defaultTransform" });
      endian::little_int16_t name;
      endian::little_int16_t parent;
      endian::little_int16_t num_sub_sequences;
      endian::little_int16_t first_sub_sequence;
      endian::little_int16_t default_transform;
    };

    struct sub_sequence
    {
      constexpr static auto keys = make_keys({ "sequenceIndex", "numKeyFrames", "firstKeyFrame" });
      endian::little_int16_t sequence_index;
      endian::little_int16_t num_key_frames;
      endian::little_int16_t first_key_frame;
    };


    struct keyframe
    {
      constexpr static auto keys = make_keys({ "position", "keyValue", "matIndex" });
      float position;
      endian::little_uint16_t key_value;
      endian::little_uint16_t mat_index;
    };

    struct transform
    {
      constexpr static auto keys = make_keys({ "rotation", "translation" });
      quaternion4s rotation;
      vector3f translation;
    };

    struct object
    {
      constexpr static auto keys = make_keys({ "nameIndex", "flags", "meshIndex", "nodeIndex", "objectOffset", "numSubSequences", "firstSubSequence" });
      endian::little_int16_t name_index;
      endian::little_int16_t flags;
      endian::little_int32_t mesh_index;
      endian::little_int16_t node_index;
      vector3f object_offset;
      endian::little_int16_t num_sub_sequences;
      endian::little_int16_t first_sub_sequence;
    };

    struct transition
    {
      constexpr static auto keys = make_keys({ "startSequence",
        "endSequence",
        "startPosition",
        "endPosition",
        "duration",
        "transformation" });
      endian::little_int32_t start_sequence;
      endian::little_int32_t end_sequence;
      float start_position;
      float end_position;
      float duration;
      transform transformation;
    };

    struct shape
    {
      constexpr static auto type_name = v2::shape::type_name;
      constexpr static auto version = 8;
      constexpr static auto keys = make_keys({ "header",
                                               "data",
                                               "nodes",
                                               "sequences",
                                               "subSequences",
                                               "keyframes",
                                               "transforms",
                                               "names",
                                               "objects",
                                               "details",
                                               "transitions",
                                               "frameTriggers",
                                               "footer",
                                               "meshes",
                                               "materialList" });

      v5::header header;
      data data;
      std::vector<node> nodes;
      std::vector<v5::sequence> sequences;
      std::vector<sub_sequence> sub_sequences;
      std::vector<keyframe> keyframes;
      std::vector<transform> transforms;
      std::vector<v2::name> names;
      std::vector<v8::object> objects;
      std::vector<v2::detail> details;
      std::vector<v8::transition> transitions;
      std::vector<v5::frame_trigger> frame_triggers;
      v5::footer footer;
      std::vector<mesh_variant> meshes;

      material_list_variant material_list;
    };
  }// namespace shape::v8
  
  using shape_variant = std::variant<shape::v2::shape, shape::v3::shape, shape::v5::shape, shape::v6::shape, shape::v7::shape, shape::v8::shape>;

  using shape_or_material_list = std::variant<material_list_variant, shape_variant>;
}// namespace darkstar::dts

#endif//DARKSTARDTSCONVERTER_STRUCTURES_HPP
