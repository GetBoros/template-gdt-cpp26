#------------------------------------------------------------------------------------------------------------
extends Control
#------------------------------------------------------------------------------------------------------------
signal start_game_requested()
#------------------------------------------------------------------------------------------------------------
@onready var _start_button: Button = %StartButton
@onready var _quit_button: Button = $VBoxContainer/QuitButton
@onready var _label_activate: Label = $Label
#------------------------------------------------------------------------------------------------------------
func _ready() -> void:
	var tween: Tween = create_tween().set_trans(Tween.TRANS_CUBIC).set_ease(Tween.EASE_OUT) # create interp
	
	tween.tween_property(_label_activate, "position:y", _label_activate.position.y - 80.0, 1.5) # move up at 80px by 1.5 sec
	tween.parallel().tween_property(_label_activate, "modulate:a", 0.0, 1.5) # in parale change transparents to 0.0 by 1.5 sec
	tween.tween_callback(_label_activate.queue_free) # at the end delete node and free mem

	_start_button.pressed.connect(On_Start_Pressed)
	_quit_button.pressed.connect(On_Quit_Pressed)

	$Rotator2D.Reset_Speed()
	
#------------------------------------------------------------------------------------------------------------
func On_Start_Pressed() -> void:
	start_game_requested.emit()

#------------------------------------------------------------------------------------------------------------
func On_Quit_Pressed() -> void:
	get_tree().quit()

#------------------------------------------------------------------------------------------------------------
