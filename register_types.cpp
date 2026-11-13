/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "register_types.h"

#include "gdscriptc.h"
#include "gdscriptc_cache.h"
#include "gdscriptc_parser.h"
#include "gdscriptc_resource_format.h"
#include "gdscriptc_tokenizer_buffer.h"
#include "gdscriptc_utility_functions.h"

#ifdef TOOLS_ENABLED
#include "editor/gdscriptc_highlighter.h"
#include "editor/gdscriptc_translation_parser_plugin.h"
#include "editor/script/script_editor_plugin.h"

#ifndef GDSCRIPTC_NO_LSP
#include "language_server/gdscriptc_language_protocol.h"
#include "language_server/gdscriptc_language_server.h"
#endif
#endif // TOOLS_ENABLED

#include "core/io/file_access.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/object/class_db.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/export/editor_export.h"
#include "editor/translations/editor_translation_parser.h"

#ifndef GDSCRIPTC_NO_LSP
#include "core/config/engine.h"
#endif
#endif // TOOLS_ENABLED


GDScriptCLanguage *script_language_gdc = nullptr;
Ref<ResourceFormatLoaderGDScriptC> resource_loader_gdc;
Ref<ResourceFormatSaverGDScriptC> resource_saver_gdc;
GDScriptCCache *gdscriptc_cache = nullptr;

#ifdef TOOLS_ENABLED

Ref<GDScriptCEditorTranslationParserPlugin> gdscriptc_translation_parser_plugin;

class EditorExportGDScriptC : public EditorExportPlugin {
	GDCLASS(EditorExportGDScriptC, EditorExportPlugin);

	static constexpr EditorExportPreset::ScriptExportMode DEFAULT_SCRIPT_MODE = EditorExportPreset::MODE_SCRIPT_BINARY_TOKENS_COMPRESSED;
	EditorExportPreset::ScriptExportMode script_mode = DEFAULT_SCRIPT_MODE;

protected:
	virtual void _export_begin(const HashSet<String> &p_features, bool p_debug, const String &p_path, int p_flags) override {
		script_mode = DEFAULT_SCRIPT_MODE;

		const Ref<EditorExportPreset> &preset = get_export_preset();
		if (preset.is_valid()) {
			script_mode = preset->get_script_export_mode();
		}
	}

	virtual void _export_file(const String &p_path, const String &p_type, const HashSet<String> &p_features) override {
		if (p_path.get_extension() != "gdsc" || script_mode == EditorExportPreset::MODE_SCRIPT_TEXT) {
			return;
		}

		Vector<uint8_t> file = FileAccess::get_file_as_bytes(p_path);
		if (file.is_empty()) {
			return;
		}

		String source = String::utf8(reinterpret_cast<const char *>(file.ptr()), file.size());
		GDScriptCTokenizerBuffer::CompressMode compress_mode = script_mode == EditorExportPreset::MODE_SCRIPT_BINARY_TOKENS_COMPRESSED ? GDScriptCTokenizerBuffer::COMPRESS_ZSTD : GDScriptCTokenizerBuffer::COMPRESS_NONE;
		file = GDScriptCTokenizerBuffer::parse_code_string(source, compress_mode);
		if (file.is_empty()) {
			return;
		}

		add_file(p_path.get_basename() + ".gdcc", file, true);
	}

public:
	virtual String get_name() const override { return "GDScriptC"; }
};

static void _editor_init() {
	Ref<EditorExportGDScriptC> gd_export;
	gd_export.instantiate();
	EditorExport::get_singleton()->add_export_plugin(gd_export);

#ifdef TOOLS_ENABLED
	Ref<GDScriptCSyntaxHighlighter> gdscriptc_syntax_highlighter;
	gdscriptc_syntax_highlighter.instantiate();
	ScriptEditor::get_singleton()->register_syntax_highlighter(gdscriptc_syntax_highlighter);
#endif
}

#endif // TOOLS_ENABLED

void initialize_gdc_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		GDREGISTER_CLASS(GDScriptC);
		GDREGISTER_INTERNAL_CLASS(GDScriptCFunctionState);

		script_language_gdc = memnew(GDScriptCLanguage);
		ScriptServer::register_language(script_language_gdc);

		resource_loader_gdc.instantiate();
		ResourceLoader::add_resource_format_loader(resource_loader_gdc);

		resource_saver_gdc.instantiate();
		ResourceSaver::add_resource_format_saver(resource_saver_gdc);

		gdscriptc_cache = memnew(GDScriptCCache);

		GDScriptCUtilityFunctions::register_functions();
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		EditorNode::add_init_callback(_editor_init);

		gdscriptc_translation_parser_plugin.instantiate();
		EditorTranslationParser::get_singleton()->add_parser(gdscriptc_translation_parser_plugin, EditorTranslationParser::STANDARD);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(GDScriptCSyntaxHighlighter);
#ifndef GDSCRIPTC_NO_LSP
		register_gdc_lsp_types();
		memnew(GDScriptCLanguageProtocol);
		EditorPlugins::add_by_type<GDScriptCLanguageServer>();

		Engine::Singleton singleton("GDScriptCLanguageProtocol", GDScriptCLanguageProtocol::get_singleton());
		singleton.editor_only = true;
		Engine::get_singleton()->add_singleton(singleton);
#endif // !GDSCRIPTC_NO_LSP
	}
#endif // TOOLS_ENABLED
}

void uninitialize_gdc_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		ScriptServer::unregister_language(script_language_gdc);

		if (gdscriptc_cache) {
			memdelete(gdscriptc_cache);
		}

		if (script_language_gdc) {
			memdelete(script_language_gdc);
		}

		ResourceLoader::remove_resource_format_loader(resource_loader_gdc);
		resource_loader_gdc.unref();

		ResourceSaver::remove_resource_format_saver(resource_saver_gdc);
		resource_saver_gdc.unref();

		GDScriptCParser::cleanup();
		GDScriptCUtilityFunctions::unregister_functions();
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorTranslationParser::get_singleton()->remove_parser(gdscriptc_translation_parser_plugin, EditorTranslationParser::STANDARD);
		gdscriptc_translation_parser_plugin.unref();
#ifndef GDSCRIPTC_NO_LSP
		memdelete(GDScriptCLanguageProtocol::get_singleton());
#endif // GDSCRIPTC_NO_LSP
	}
#endif // TOOLS_ENABLED
}
