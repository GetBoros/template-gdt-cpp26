#------------------------------------------------------------------------------------------------------------
extends Node
#------------------------------------------------------------------------------------------------------------
const MAIN_MENU_SCENE: PackedScene = preload("res://ui/main_menu/MainMenu.tscn")
const LEVEL_TEST_SCENE: PackedScene = preload("res://gameplay/Level_Test.tscn")

@onready var _world: Node2D = $World
@onready var _ui: CanvasLayer = $UI
#------------------------------------------------------------------------------------------------------------
func _ready() -> void:
	_show_main_menu()
#------------------------------------------------------------------------------------------------------------
func _show_main_menu() -> void:
	var menu_instance: Control = MAIN_MENU_SCENE.instantiate()
	
	_ui.add_child(menu_instance)
	menu_instance.start_game_requested.connect(_on_start_game_requested)

#------------------------------------------------------------------------------------------------------------
func _on_start_game_requested() -> void:
	print("Game: Получен сигнал старта! Загружаем уровень...")
	
	for child in _ui.get_children():
		child.queue_free()

	var level_instance: Node2D = LEVEL_TEST_SCENE.instantiate()
	_world.add_child(level_instance)

#------------------------------------------------------------------------------------------------------------
func _process(delta: float) -> void:
	pass
#------------------------------------------------------------------------------------------------------------
# 1. _init()  -> C++ Construct / create instance in memory
# 2. _enter_tree()  -> Node enter in tree SceneTree (Parent -> Child)
# 3. _ready()  -> Node and all her child ready to work (Child -> Parent!)
# 4. _process(delta)  -> Tick, (fps, VSync)
#    _physics_process(delta)  -> Physic tick (by defoult 60 Hz)
# 5. _exit_tree()  -> Node removed from tree (down up)
# 6. _notification(NOTIFICATION_PREDELETE)  -> C++ destruct
#------------------------------------------------------------------------------------------------------------
# [SceneTree] (Game manager game cycle singlton)
#    │
#    └── root (Window)
#          │
#          └── Game (Node)
#                ├── WorldContainer (Node2D)
#                │     └── Player (Node2D)
#                └── UIContainer (CanvasLayer)
#                      └── MainMenu (Control)
#------------------------------------------------------------------------------------------------------------
