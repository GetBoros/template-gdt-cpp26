extends Label

#------------------------------------------------------------------------------------------------------------
func _ready() -> void:
	# _ready() — это аналог BeginPlay() в Unreal Engine
	# Создаем твин (интерполятор)
	var tween: Tween = create_tween()
	
	# 1. Сдвигаем по оси Y вверх на 80 пикселей за 1.5 секунды
	tween.tween_property(self, "position:y", position.y - 80.0, 1.5)
	
	# 2. Параллельно с движением плавно растворяем альфа-канал в 0
	tween.parallel().tween_property(self, "modulate:a", 0.0, 1.5)
	
	# 3. Когда анимация завершилась — удаляем ноду из памяти (аналог DestroyActor / MarkPendingKill)
	tween.tween_callback(queue_free)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

# Lounch Godot Editor godot -e
# Lounch Godot godot --print-fps  // show fps in console
