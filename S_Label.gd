#------------------------------------------------------------------------------------------------------------
extends Label
#------------------------------------------------------------------------------------------------------------
func _ready() -> void:
	var tween: Tween = create_tween() # create interp
	
	tween.tween_property(self, "position:y", position.y - 80.0, 1.5) # move up at 80px by 1.5 sec
	tween.parallel().tween_property(self, "modulate:a", 0.0, 1.5) # in parale change transparents to 0.0 by 1.5 sec
	tween.tween_callback(queue_free) # at the end delete node and free mem
#------------------------------------------------------------------------------------------------------------
func _process(delta: float) -> void:
	pass
#------------------------------------------------------------------------------------------------------------
