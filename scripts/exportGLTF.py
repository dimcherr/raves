import bpy
import sys

try:
    argv = sys.argv
    bpy.context.preferences.addons['io_scene_gltf2'].preferences.KHR_materials_variants_ui = True

    for obj in bpy.context.scene.objects:
        obj.hide_viewport = False

    # Export the scene as glTF
    bpy.ops.export_scene.gltf(
        filepath=argv[5],
        export_format='GLB',  # Options: 'GLTF_SEPARATE', 'GLTF_EMBEDDED', 'GLB'
        use_selection=False,            # Export entire scene; set to True for selected objects only
        use_visible=False,
        export_animations=False,
        export_vertex_color="NONE",
        export_all_vertex_colors=False,
        export_tangents=True,
        export_lights=True,
        export_extras=True,
        export_apply=True,
        export_materials='EXPORT',
        export_yup=True,
        export_hierarchy_flatten_objs=False
    )

    sys.exit(0)
except Exception as err:
    print(err, file=sys.stderr)
    sys.exit(1)
