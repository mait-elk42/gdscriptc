# GDScriptC (.gdsc)

A C-like dialect of GDScript for the Godot Engine. Same engine, same runtime,
different syntax: **braces `{ }`**, **semicolons `;`**, and **required parentheses**.

It compiles to the same bytecode as GDScript, so every Godot API, type, and
feature works identically — you're only changing how you write the code.

## Example

```gdscript
extends CharacterBody2D;

@export var speed := 300.0;

enum State { IDLE, RUN };

var state := State.IDLE;

func _physics_process(delta: float) -> void {
	match (state) {
		State.IDLE: {
			if (Input.is_action_pressed("move")) {
				state = State.RUN;
			}
		}
		State.RUN: {
			var dir := Input.get_vector("left", "right", "up", "down");
			velocity = dir * speed;
			move_and_slide();
		}
	}
}
```

## Syntax reference

| Construct | Syntax |
|---|---|
| Blocks | `{ }` — no colon before them |
| Statement terminator | `;` (newlines are insignificant) |
| `if` / `elif` / `while` | `(condition)` **required** |
| `for` | unchanged foreach: `for i in range(10) { }` |
| `match` | `match (subject) { pattern: { } }` — patterns keep `:` |
| Function | `func name(args) -> T { }` |
| Type annotation | `:` — `var x: int;` |
| Dictionary | `{ "a": 1 }` (braces overloaded, like JS) |
| Enum | `enum State { A, B };` |
| Lambda | `func(x) { return x * 2; }` |
| Single-statement body | `if (x) foo();` — braces optional for one statement |
| Empty body | `func f() { }` — allowed |

## Differences from GDScript

1. **Indentation → braces.** No INDENT/DEDENT; blocks are `{ }`.
2. **Block colon removed** (kept for type annotations, dict keys, `match` patterns).
3. **Newline → `;`.** Newlines are whitespace.
4. **Conditions require `( )`** for `if`/`elif`/`while`/`match`.
5. **Empty function bodies allowed** (GDScript requires `pass`).
6. **Single-statement bodies allowed** without braces.

## Not (yet) supported

- C-style `for (int i = 0; i < n; i++)` — use `for i in range(n)`.
- `++` / `--` — use `i += 1`.
- String interpolation `"{}"` — use `%`: `"hello %s" % name`.

## Install

Requires a Godot source checkout (built-in GDScript module must be present —
GDScriptC is forked from it and coexists alongside it).

```bash
# clone this repo as the "gdc" module
git clone https://github.com/you/gdscriptc custom_modules/gdc

# build the editor
scons platform=linuxbsd target=editor custom_modules=custom_modules -j$(nproc)
```

Then attach `.gdsc` scripts like `.gd` scripts. To run one standalone:

```bash
godot --headless --script path/to/script.gdsc
```

## Compatibility

This module is forked from **Godot 4.7.x**'s GDScript module. It works with the
matching Godot version. Other 4.x/3.x versions may not compile, because Godot's
internal GDScript APIs change between releases.

## Structure

```
gdc/
├── config.py                 # module registration
├── SCsub                     # build script
├── register_types.cpp/h      # language registration
├── gdscriptc_*.cpp/h         # tokenizer, parser, analyzer, compiler, VM
├── editor/                   # highlighter, docgen, script templates
└── language_server/          # LSP (completion/hover/go-to-definition)
```

## License

MIT — forked from the Godot Engine (also MIT). See `LICENSE`.

