/**************************************************************************/
/*  gdscriptc_cache.h                                                      */
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

#pragma once

#include "gdscriptc.h"

#include "core/object/ref_counted.h"
#include "core/os/safe_binary_mutex.h"
#include "core/templates/hash_map.h"
#include "core/templates/hash_set.h"

class GDScriptCAnalyzer;
class GDScriptCParser;

class GDScriptCParserRef : public RefCounted {
	GDSOFTCLASS(GDScriptCParserRef, RefCounted);

public:
	enum Status {
		EMPTY,
		PARSED,
		INHERITANCE_SOLVED,
		INTERFACE_SOLVED,
		FULLY_SOLVED,
	};

private:
	GDScriptCParser *parser = nullptr;
	GDScriptCAnalyzer *analyzer = nullptr;
	Status status = EMPTY;
	Error result = OK;
	String path;
	uint32_t source_hash = 0;
	bool clearing = false;
	bool abandoned = false;

	friend class GDScriptCCache;
	friend class GDScriptC;

public:
	Status get_status() const;
	String get_path() const;
	uint32_t get_source_hash() const;
	GDScriptCParser *get_parser();
	GDScriptCAnalyzer *get_analyzer();
	Error raise_status(Status p_new_status);
	void clear();

	GDScriptCParserRef() {}
	~GDScriptCParserRef();
};

namespace GDScriptCTests {
class TestGDScriptCCacheAccessor;
}

class GDScriptCCache {
	// String key is full path.
	HashMap<String, GDScriptCParserRef *> parser_map;
	HashMap<String, Vector<ObjectID>> abandoned_parser_map;
	HashMap<String, Ref<GDScriptC>> shallow_gdscriptc_cache;
	HashMap<String, Ref<GDScriptC>> full_gdscriptc_cache;
	HashMap<String, Ref<GDScriptC>> static_gdscriptc_cache;
	HashMap<String, HashSet<String>> dependencies;
	HashMap<String, HashSet<String>> parser_inverse_dependencies;

	friend class GDScriptC;
	friend class GDScriptCParserRef;
	friend class GDScriptCInstance;
	friend class GDScriptCTests::TestGDScriptCCacheAccessor;

	static GDScriptCCache *singleton;

	bool cleared = false;

public:
	static const int BINARY_MUTEX_TAG = 3;

private:
	static SafeBinaryMutex<BINARY_MUTEX_TAG> mutex;
	friend SafeBinaryMutex<BINARY_MUTEX_TAG> &_get_gdscriptc_cache_mutex();

public:
	static void move_script(const String &p_from, const String &p_to);
	static void remove_script(const String &p_path);
	static Ref<GDScriptCParserRef> get_parser(const String &p_path, GDScriptCParserRef::Status status, Error &r_error, const String &p_owner = String());
	static bool has_parser(const String &p_path);
	static void remove_parser(const String &p_path);
	static String get_source_code(const String &p_path);
	static Vector<uint8_t> get_binary_tokens(const String &p_path);
	static Ref<GDScriptC> get_shallow_script(const String &p_path, Error &r_error, const String &p_owner = String());
	/**
	 * Returns a fully loaded GDScriptC using an already cached script if one exists.
	 *
	 * The returned instance is present in GDScriptCCache and ResourceCache.
	 */
	static Ref<GDScriptC> get_full_script(const String &p_path, Error &r_error, const String &p_owner = String(), bool p_update_from_disk = false);
	static Ref<GDScriptC> get_cached_script(const String &p_path);
	static Error finish_compiling(const String &p_owner);
	static void add_static_script(Ref<GDScriptC> p_script);
	static void remove_static_script(const String &p_fqcn);

	static void clear();

	GDScriptCCache();
	~GDScriptCCache();
};
