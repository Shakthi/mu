//: Allow mu programs to log facts just like we've been doing in C++ so far.

:(scenario trace)
recipe main [
  trace [foo], [this is a trace in mu]
]
+foo: this is a trace in mu

:(before "End Primitive Recipe Declarations")
TRACE,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["trace"] = TRACE;
:(before "End Primitive Recipe Implementations")
case TRACE: {
  if (SIZE(ingredients) == 2) {
    assert(is_literal(current_instruction().ingredients.at(0)));
    string label = current_instruction().ingredients.at(0).name;
    assert(is_literal(current_instruction().ingredients.at(1)));
    string message = current_instruction().ingredients.at(1).name;
    trace(1, label) << message << end();
  }
  else if (SIZE(ingredients) == 1) {
    assert(is_literal(current_instruction().ingredients.at(0)));
    string message = current_instruction().ingredients.at(0).name;
    cerr << "tracing " << message << '\n';
    trace(1, "app") << message << end();
  }
  else {
    raise << current_recipe_name() << ": 'trace' takes one or two ingredients rather than '" << current_instruction().to_string() << "'\n" << end();
  }
  break;
}

:(before "End Primitive Recipe Declarations")
HIDE_WARNINGS,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["hide-warnings"] = HIDE_WARNINGS;
:(before "End Primitive Recipe Implementations")
case HIDE_WARNINGS: {
  Hide_warnings = true;
  break;
}

:(before "End Primitive Recipe Declarations")
SHOW_WARNINGS,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["show-warnings"] = SHOW_WARNINGS;
:(before "End Primitive Recipe Implementations")
case SHOW_WARNINGS: {
  Hide_warnings = false;
  break;
}

:(before "End Primitive Recipe Declarations")
_START_TRACING,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$start-tracing"] = _START_TRACING;
:(before "End Primitive Recipe Implementations")
case _START_TRACING: {
  if (current_instruction().ingredients.empty())
    Trace_stream->dump_layer = "all";
  else
    Trace_stream->dump_layer = current_instruction().ingredients.at(0).name;
//?   cout << Trace_stream << ": " << Trace_stream->dump_layer << '\n'; //? 1
  break;
}

:(before "End Primitive Recipe Declarations")
_STOP_TRACING,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$stop-tracing"] = _STOP_TRACING;
:(before "End Primitive Recipe Implementations")
case _STOP_TRACING: {
  Trace_stream->dump_layer = "";
  break;
}

:(before "End Primitive Recipe Declarations")
_CLOSE_TRACE,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$close-trace"] = _CLOSE_TRACE;
:(before "End Primitive Recipe Implementations")
case _CLOSE_TRACE: {
  if (Trace_stream) {
    delete Trace_stream;
    Trace_stream = NULL;
  }
  break;
}

:(before "End Primitive Recipe Declarations")
_DUMP_TRACE,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$dump-trace"] = _DUMP_TRACE;
:(before "End Primitive Recipe Implementations")
case _DUMP_TRACE: {
  if (ingredients.empty()) {
    DUMP("");
  }
  else {
    DUMP(current_instruction().ingredients.at(0).name);
  }
  break;
}

:(before "End Primitive Recipe Declarations")
_CLEAR_TRACE,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$clear-trace"] = _CLEAR_TRACE;
:(before "End Primitive Recipe Implementations")
case _CLEAR_TRACE: {
  CLEAR_TRACE;
  break;
}

//: assert: perform sanity checks at runtime

:(scenario assert)
% Hide_warnings = true;  // '%' lines insert arbitrary C code into tests before calling 'run' with the lines below. Must be immediately after :(scenario) line.
recipe main [
  assert 0, [this is an assert in mu]
]
+warn: this is an assert in mu

:(before "End Primitive Recipe Declarations")
ASSERT,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["assert"] = ASSERT;
:(before "End Primitive Recipe Implementations")
case ASSERT: {
  if (SIZE(ingredients) != 2) {
    raise << current_recipe_name() << ": 'assert' takes exactly two ingredients rather than '" << current_instruction().to_string() << "'\n" << end();
    break;
  }
  if (!scalar(ingredients.at(0))) {
    raise << current_recipe_name() << ": 'assert' requires a boolean for its first ingredient, but got " << current_instruction().ingredients.at(0).original_string << '\n' << end();
    break;
  }
  if (!scalar(ingredients.at(1))) {
    raise << current_recipe_name() << ": 'assert' requires a literal string for its second ingredient, but got " << current_instruction().ingredients.at(1).original_string << '\n' << end();
    break;
  }
  if (!ingredients.at(0).at(0)) {
    raise << current_instruction().ingredients.at(1).name << '\n' << end();
  }
  break;
}

//:: 'cheating' by using the host system

:(before "End Primitive Recipe Declarations")
_PRINT,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$print"] = _PRINT;
:(before "End Primitive Recipe Implementations")
case _PRINT: {
  for (long long int i = 0; i < SIZE(ingredients); ++i) {
    if (is_literal(current_instruction().ingredients.at(i))) {
      trace(Primitive_recipe_depth, "run") << "$print: " << current_instruction().ingredients.at(i).name << end();
      if (has_property(current_instruction().ingredients.at(i), "newline"))
        cout << '\n';
      else
        cout << current_instruction().ingredients.at(i).name;
    }
    else {
      for (long long int j = 0; j < SIZE(ingredients.at(i)); ++j) {
        trace(Primitive_recipe_depth, "run") << "$print: " << ingredients.at(i).at(j) << end();
        if (j > 0) cout << " ";
        cout << ingredients.at(i).at(j);
      }
    }
  }
  break;
}

:(before "End Primitive Recipe Declarations")
_EXIT,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$exit"] = _EXIT;
:(before "End Primitive Recipe Implementations")
case _EXIT: {
  exit(0);
  break;
}

:(before "End Primitive Recipe Declarations")
_SYSTEM,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$system"] = _SYSTEM;
:(before "End Primitive Recipe Implementations")
case _SYSTEM: {
  products.resize(1);
  if (current_instruction().ingredients.empty()) {
    raise << current_recipe_name() << ": '$system' requires exactly one ingredient, but got none\n" << end();
    break;
  }
  int status = system(current_instruction().ingredients.at(0).name.c_str());
  products.at(0).push_back(status);
  break;
}

//:: helpers for debugging

:(before "End Primitive Recipe Declarations")
_DUMP_MEMORY,
:(before "End Primitive Recipe Numbers")
Recipe_ordinal["$dump-memory"] = _DUMP_MEMORY;
:(before "End Primitive Recipe Implementations")
case _DUMP_MEMORY: {
  dump_memory();
  break;
}
