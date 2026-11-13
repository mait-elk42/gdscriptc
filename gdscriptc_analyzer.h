/**************************************************************************/
/*  gdscriptc_analyzer.h                                                   */
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

#include "gdscriptc_cache.h"
#include "gdscriptc_parser.h"

#include "core/object/ref_counted.h"

class GDScriptCAnalyzer {
	GDScriptCParser *parser = nullptr;

	template <typename Fn>
	class Finally {
		Fn fn;

	public:
		Finally(Fn p_fn) :
				fn(p_fn) {}
		~Finally() {
			fn();
		}
	};

	const GDScriptCParser::EnumNode *current_enum = nullptr;
	GDScriptCParser::LambdaNode *current_lambda = nullptr;
	List<GDScriptCParser::LambdaNode *> pending_body_resolution_lambdas;
	HashMap<const GDScriptCParser::ClassNode *, Ref<GDScriptCParserRef>> external_class_parser_cache;
	bool static_context = false;

	// Tests for detecting invalid overloading of script members
	static _FORCE_INLINE_ bool has_member_name_conflict_in_script_class(const StringName &p_name, const GDScriptCParser::ClassNode *p_current_class_node, const GDScriptCParser::Node *p_member);
	static _FORCE_INLINE_ bool has_member_name_conflict_in_native_type(const StringName &p_name, const StringName &p_native_type_string);
	Error check_native_member_name_conflict(const StringName &p_member_name, const GDScriptCParser::Node *p_member_node, const StringName &p_native_type_string);
	Error check_class_member_name_conflict(const GDScriptCParser::ClassNode *p_class_node, const StringName &p_member_name, const GDScriptCParser::Node *p_member_node);

	void get_class_node_current_scope_classes(GDScriptCParser::ClassNode *p_node, List<GDScriptCParser::ClassNode *> *p_list, GDScriptCParser::Node *p_source);

	Error resolve_class_inheritance(GDScriptCParser::ClassNode *p_class, const GDScriptCParser::Node *p_source = nullptr);
	Error resolve_class_inheritance(GDScriptCParser::ClassNode *p_class, bool p_recursive);
	GDScriptCParser::DataType resolve_datatype(GDScriptCParser::TypeNode *p_type);

	void decide_suite_type(GDScriptCParser::Node *p_suite, GDScriptCParser::Node *p_statement);

	void resolve_annotation(GDScriptCParser::AnnotationNode *p_annotation);
	void resolve_class_member(GDScriptCParser::ClassNode *p_class, const StringName &p_name, const GDScriptCParser::Node *p_source = nullptr);
	void resolve_class_member(GDScriptCParser::ClassNode *p_class, int p_index, const GDScriptCParser::Node *p_source = nullptr);
	void resolve_class_interface(GDScriptCParser::ClassNode *p_class, const GDScriptCParser::Node *p_source = nullptr);
	void resolve_class_interface(GDScriptCParser::ClassNode *p_class, bool p_recursive);
	void resolve_class_body(GDScriptCParser::ClassNode *p_class, const GDScriptCParser::Node *p_source = nullptr);
	void resolve_class_body(GDScriptCParser::ClassNode *p_class, bool p_recursive);
	void resolve_function_signature(GDScriptCParser::FunctionNode *p_function, const GDScriptCParser::Node *p_source = nullptr, bool p_is_lambda = false);
	void resolve_function_body(GDScriptCParser::FunctionNode *p_function, bool p_is_lambda = false);
	void resolve_node(GDScriptCParser::Node *p_node, bool p_is_root = true);
	void resolve_suite(GDScriptCParser::SuiteNode *p_suite, bool p_is_root = true);
	void resolve_assignable(GDScriptCParser::AssignableNode *p_assignable, const char *p_kind);
	void resolve_variable(GDScriptCParser::VariableNode *p_variable, bool p_is_local);
	void resolve_constant(GDScriptCParser::ConstantNode *p_constant, bool p_is_local);
	void resolve_parameter(GDScriptCParser::ParameterNode *p_parameter);
	void resolve_if(GDScriptCParser::IfNode *p_if);
	void resolve_for(GDScriptCParser::ForNode *p_for);
	void resolve_while(GDScriptCParser::WhileNode *p_while);
	void resolve_assert(GDScriptCParser::AssertNode *p_assert);
	void resolve_match(GDScriptCParser::MatchNode *p_match);
	void resolve_match_branch(GDScriptCParser::MatchBranchNode *p_match_branch, GDScriptCParser::ExpressionNode *p_match_test);
	void resolve_match_pattern(GDScriptCParser::PatternNode *p_match_pattern, GDScriptCParser::ExpressionNode *p_match_test);
	void resolve_return(GDScriptCParser::ReturnNode *p_return);

	// Reduction functions.
	void reduce_expression(GDScriptCParser::ExpressionNode *p_expression, bool p_is_root = false);
	void reduce_array(GDScriptCParser::ArrayNode *p_array);
	void reduce_assignment(GDScriptCParser::AssignmentNode *p_assignment);
	void reduce_await(GDScriptCParser::AwaitNode *p_await);
	void reduce_binary_op(GDScriptCParser::BinaryOpNode *p_binary_op);
	void reduce_call(GDScriptCParser::CallNode *p_call, bool p_is_await = false, bool p_is_root = false);
	void reduce_cast(GDScriptCParser::CastNode *p_cast);
	void reduce_dictionary(GDScriptCParser::DictionaryNode *p_dictionary);
	void reduce_get_node(GDScriptCParser::GetNodeNode *p_get_node);
	void reduce_identifier(GDScriptCParser::IdentifierNode *p_identifier, bool can_be_builtin = false);
	void reduce_identifier_from_base(GDScriptCParser::IdentifierNode *p_identifier, GDScriptCParser::DataType *p_base = nullptr);
	void reduce_lambda(GDScriptCParser::LambdaNode *p_lambda);
	void reduce_literal(GDScriptCParser::LiteralNode *p_literal);
	void reduce_preload(GDScriptCParser::PreloadNode *p_preload);
	void reduce_self(GDScriptCParser::SelfNode *p_self);
	void reduce_subscript(GDScriptCParser::SubscriptNode *p_subscript, bool p_can_be_pseudo_type = false);
	void reduce_ternary_op(GDScriptCParser::TernaryOpNode *p_ternary_op, bool p_is_root = false);
	void reduce_type_test(GDScriptCParser::TypeTestNode *p_type_test);
	void reduce_unary_op(GDScriptCParser::UnaryOpNode *p_unary_op);

	// These methods provide a fallback mechanism for constant folding in constant contexts (constant initializers,
	// annotation arguments) and allow constant folding even when the normal constant folding mechanism does not consider
	// an expression to be constant (usually due to the presence of `[...]` and `{...}` constructs in it).
	// These methods assume that the normal expression reduction mechanism has already been performed.
	Variant make_expression_reduced_value(GDScriptCParser::ExpressionNode *p_expression, bool &r_is_reduced);
	Variant make_array_reduced_value(GDScriptCParser::ArrayNode *p_array, bool &r_is_reduced);
	Variant make_dictionary_reduced_value(GDScriptCParser::DictionaryNode *p_dictionary, bool &r_is_reduced);
	Variant make_subscript_reduced_value(GDScriptCParser::SubscriptNode *p_subscript, bool &r_is_reduced);
	Variant make_call_reduced_value(GDScriptCParser::CallNode *p_call, bool &r_is_reduced);
	Variant make_binary_op_reduced_value(GDScriptCParser::BinaryOpNode *p_binary_op, bool &r_is_reduced);
	Variant make_ternary_op_reduced_value(GDScriptCParser::TernaryOpNode *p_ternary_op, bool &r_is_reduced);
	Variant make_cast_reduced_value(GDScriptCParser::CastNode *p_cast, bool &r_is_reduced);
	Variant make_type_test_reduced_value(GDScriptCParser::TypeTestNode *p_type_test, bool &r_is_reduced);

	// Helpers.
	Array make_array_from_element_datatype(const GDScriptCParser::DataType &p_element_datatype, const GDScriptCParser::Node *p_source_node = nullptr);
	Dictionary make_dictionary_from_element_datatype(const GDScriptCParser::DataType &p_key_element_datatype, const GDScriptCParser::DataType &p_value_element_datatype, const GDScriptCParser::Node *p_source_node = nullptr);
	GDScriptCParser::DataType type_from_script(const Ref<Script> &p_script, const GDScriptCParser::Node *p_source, bool p_is_meta_type);
	GDScriptCParser::DataType type_from_variant(const Variant &p_value, const GDScriptCParser::Node *p_source);
	GDScriptCParser::DataType type_from_property_hint_string(const String &p_type_name) const;
	GDScriptCParser::DataType type_from_property(const PropertyInfo &p_property, bool p_is_arg = false, bool p_is_readonly = false) const;
	GDScriptCParser::DataType make_global_class_meta_type(const StringName &p_class_name, const GDScriptCParser::Node *p_source);
	bool get_function_signature(GDScriptCParser::Node *p_source, bool p_is_constructor, GDScriptCParser::DataType base_type, const StringName &p_function, GDScriptCParser::DataType &r_return_type, List<GDScriptCParser::DataType> &r_par_types, int &r_default_arg_count, BitField<MethodFlags> &r_method_flags, StringName *r_native_class = nullptr);
	bool function_signature_from_info(const MethodInfo &p_info, GDScriptCParser::DataType &r_return_type, List<GDScriptCParser::DataType> &r_par_types, int &r_default_arg_count, BitField<MethodFlags> &r_method_flags);
	void validate_call_arg(const List<GDScriptCParser::DataType> &p_par_types, int p_default_args_count, bool p_is_vararg, const GDScriptCParser::CallNode *p_call);
	void validate_call_arg(const MethodInfo &p_method, const GDScriptCParser::CallNode *p_call);
	GDScriptCParser::DataType get_operation_type(Variant::Operator p_operation, const GDScriptCParser::DataType &p_a, const GDScriptCParser::DataType &p_b, bool &r_valid, const GDScriptCParser::Node *p_source);
	GDScriptCParser::DataType get_operation_type(Variant::Operator p_operation, const GDScriptCParser::DataType &p_a, bool &r_valid, const GDScriptCParser::Node *p_source);
	void update_const_expression_builtin_type(GDScriptCParser::ExpressionNode *p_expression, const GDScriptCParser::DataType &p_type, const char *p_usage, bool p_is_cast = false);
	void update_array_literal_element_type(GDScriptCParser::ArrayNode *p_array, const GDScriptCParser::DataType &p_element_type);
	void update_dictionary_literal_element_type(GDScriptCParser::DictionaryNode *p_dictionary, const GDScriptCParser::DataType &p_key_element_type, const GDScriptCParser::DataType &p_value_element_type);
	bool is_type_compatible(const GDScriptCParser::DataType &p_target, const GDScriptCParser::DataType &p_source, bool p_allow_implicit_conversion = false, const GDScriptCParser::Node *p_source_node = nullptr);
	bool is_type_compatible_strict_collections(const GDScriptCParser::DataType &p_target, const GDScriptCParser::DataType &p_source);
	void push_error(const String &p_message, const GDScriptCParser::Node *p_origin = nullptr);
	void mark_node_unsafe(const GDScriptCParser::Node *p_node);
	void downgrade_node_type_source(GDScriptCParser::Node *p_node);
	void mark_lambda_use_self();
	void resolve_pending_lambda_bodies();
	void reduce_identifier_from_base_set_class(GDScriptCParser::IdentifierNode *p_identifier, GDScriptCParser::DataType p_identifier_datatype);
	Ref<GDScriptCParserRef> ensure_cached_external_parser_for_class(const GDScriptCParser::ClassNode *p_class, const GDScriptCParser::ClassNode *p_from_class, const char *p_context, const GDScriptCParser::Node *p_source);
	Ref<GDScriptCParserRef> find_cached_external_parser_for_class(const GDScriptCParser::ClassNode *p_class, const Ref<GDScriptCParserRef> &p_dependant_parser);
	Ref<GDScriptCParserRef> find_cached_external_parser_for_class(const GDScriptCParser::ClassNode *p_class, GDScriptCParser *p_dependant_parser);
	Ref<GDScriptC> get_depended_shallow_script(const String &p_path, Error &r_error);

#ifdef DEBUG_ENABLED
	void is_shadowing(GDScriptCParser::IdentifierNode *p_identifier, const String &p_context, const bool p_in_local_scope);
	void warn_confusable_temporary_modification(GDScriptCParser::ExpressionNode *p_expression);
#endif // DEBUG_ENABLED

public:
	Error resolve_inheritance();
	Error resolve_interface();
	Error resolve_body();
	Error resolve_dependencies();
	Error analyze();

	Variant make_variable_default_value(GDScriptCParser::VariableNode *p_variable);

	static bool check_type_compatibility(const GDScriptCParser::DataType &p_target, const GDScriptCParser::DataType &p_source, bool p_allow_implicit_conversion = false, const GDScriptCParser::Node *p_source_node = nullptr);
	static GDScriptCParser::DataType type_from_metatype(const GDScriptCParser::DataType &p_meta_type);
	static bool class_exists(const StringName &p_class);

	GDScriptCAnalyzer(GDScriptCParser *p_parser);
};
