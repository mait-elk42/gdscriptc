# GDScriptC (.gdsc)

GDScript, but with **`{ }` braces** and **`;` semicolons** instead of indentation.
Everything else is the same Godot you know.

## Full example (shows every change)

```gdscript
# GDScriptC: braces instead of indentation, ";" instead of newlines.

extends Node2D;                       # statements end with ";"

@export var speed := 300.0;           # annotations are unchanged
@export var jump_velocity := -400.0;

signal died(amount: int);             # ";" after signal

const GRAVITY := 9.8;                 # ";" after const

enum State { IDLE, RUN, DEAD };       # enum ends with ";"

var health := 100;
var state := State.IDLE;

var inventory := {                    # dictionaries still use { }
	"sword": 1,
	"potion": 3,
};

# Functions: { } and no ":" before the body.
func _ready() -> void {
	print("hello");
}

# if / elif / else: parentheses are REQUIRED.
func take_damage(amount: int) -> void {
	health -= amount;
	if (health <= 0) {
		die();
	} elif (health < 25) {
		warn();
	} else {
		heal();
	}
}

# while: parentheses are REQUIRED.
func regen() -> void {
	while (health < 100) {
		health += 1;
	}
}

# for: kept as GDScript's foreach — NO parentheses.
func _process(delta: float) -> void {
	for enemy in get_enemies() {
		enemy.hurt(1);
	}
	for i in range(10) {
		print(i);
	}
}

# match: subject in ( ), patterns keep ":", bodies use { }.
func choose_action() -> void {
	match (state) {
		State.IDLE: idle();
		State.RUN: run();
		State.DEAD: {
			play_death();
			queue_free();
		}
		_: pass;
	}
}

# Lambdas use { }.
func make_multiplier(factor: int) -> Callable {
	var f = func(x) { return x * factor; };
	return f;
}

# Properties (get/set) use { }.
var max_health: int {
	get {
		return max_health;
	}
	set(value) {
		max_health = value;
	}
}

# Inner classes use { }.
class Helper {
	func double(x: int) -> int {
		return x * 2;
	}
}

# Single statements don't need braces.
func quick_check() -> void {
	if (health > 0) print("alive"); else print("dead");
	while (running) tick();
	for i in range(3) print(i);
}
```

## The rules (that's the whole language)

| Change | GDScript | GDScriptC |
|---|---|---|
| Blocks | `:` + indentation | `{ }` |
| End of statement | newline | `;` |
| `if` / `elif` / `while` | `if x:` | `if (x) { }` |
| `for` | `for i in range(10):` | `for i in range(10) { }` (unchanged!) |
| `match` | `match x:` | `match (x) { }` |

Still uses `:` only for: type annotations (`var x: int`), dictionary keys, and `match` patterns.

## What's NOT included

- C-style `for (int i = 0; i < n; i++)` → use `for i in range(n)`
- `++` / `--` → use `i += 1`
- `"{}"` string interpolation → use `"hello %s" % name`

## Install

```bash
git clone https://github.com/you/gdscriptc custom_modules/gdc
scons platform=linuxbsd target=editor custom_modules=custom_modules
```

Attach `.gdsc` scripts like `.gd` scripts.

## License

MIT — forked from the Godot Engine (MIT). See `LICENSE`.

