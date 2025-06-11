// Элементы модальных окон
const propertyModal = document.getElementById('property-modal');
const loadingOverlay = document.getElementById('loading-overlay');
const propertyValueInput = document.getElementById('property-value');
const errorMessage = document.getElementById('error-message');

var currentProperty = 0;



function _post_property(id, val) {
	return fetch(`/property/${id}`, {
		method: "POST",
		headers: {
			"Content-type": "application/x-www-form-urlencoded"
		},
		body: `value=${val}`
	});
}

async function _post_with_dialog(id, val) {
	openLoadingModal();

	const response = await _post_property(id, val);
	if (await response.status != 200) {
		console.log("failed response, status: " + response.status)
	}

	closeLoadingModal();
	window.location.reload();
}


// Функция для открытия модального окна изменения свойства
function openPropertyModal(currentValue = '') {
	propertyValueInput.value = currentValue;
	errorMessage.style.display = 'none';
	propertyModal.style.display = 'flex';
}

// Функция для закрытия модального окна изменения свойства
function closePropertyModal() {
	propertyModal.style.display = 'none';

}

function openLoadingModal() {
	loadingOverlay.style.display = 'flex';
}

function closeLoadingModal() {
	loadingOverlay.style.display = 'none';
}

function showError(msg) {
	errorMessage.style.display = 'flex';
	errorMessage.textContent = msg;
}



function onPropertyClick(propId) {
	currentProperty = parseInt(propId);
	openPropertyModal(document.getElementById("item-value-" + propId).value);

}

function onChangeCancel() {
	closePropertyModal();
}

function onChangeApply() {
	if (!propertyValueInput.value) {
		showError("Не допустимое значение");
		return;
	}

	_post_with_dialog(currentProperty, propertyValueInput.value)
}

function onAutoButtonClicked(propId, state) {
	_post_with_dialog(propId, state)
}

function onChangeColorClicked() {
	var rgb_color = hexToIntColor(selectedColor)
	var white_color = 255;
	var color = (white_color << 24) + rgb_color
	console.log(`Color: r ${(rgb_color >> 16) & 0xFF}, g ${(rgb_color >> 8) & 0xFF}, b ${(rgb_color) & 0xFF}`)
	
	_post_with_dialog(currentProperty, color);
}

function hexToIntColor(toConvert)
{
	return parseInt(toConvert.substring(1), 16);
}

function intToHexColor(toConvert)
{
	// toConvert = toConvert >>> 0;
	var w = (toConvert >> 24) & 0xFF;
	var r = (toConvert >> 16) & 0xFF;
	var g = (toConvert >> 8) & 0xFF;
	var b = toConvert & 0xFF;

	console.log(`Color: r ${r}, g ${g}, b ${b}, w ${w}`)


	const toHex = x => {
		const hex = x.toString(16);
		return hex.length === 1 ? '0' + hex : hex;
	};

	return `#${toHex(r)}${toHex(g)}${toHex(b)}`
}



let selectedColor = '#FFFFFF';

function initColorPicker() {
	const colorWheel = document.getElementById('colorWheel');
	const colorSelector = document.getElementById('colorSelector');
	const colorPreview = document.getElementById('colorPreview');
	const colorSelected = document.getElementById('colorSelected');
	const colorHex = document.getElementById('colorHex');
	const colorPickerModal = document.getElementById('colorPickerModal');
	const colorCancelBtn = document.getElementById('colorCancelBtn');
	const colorConfirmBtn = document.getElementById('colorConfirmBtn');

	// Обработчик движения мыши по цветовому кругу
	function handleColorSelect(e) {
		const rect = colorWheel.getBoundingClientRect();
		const x = e.clientX - rect.left;
		const y = e.clientY - rect.top;
		const centerX = rect.width / 2;
		const centerY = rect.height / 2;

		// Позиционируем селектор
		colorSelector.style.left = x + 'px';
		colorSelector.style.top = y + 'px';

		// Вычисляем цвет
		const dx = x - centerX;
		const dy = y - centerY;
		const distance = Math.sqrt(dx * dx + dy * dy);
		const radius = centerX;
		const angle = Math.atan2(dy, dx) + Math.PI / 2;

		if (distance <= radius) {
			// Нормализуем координаты от -1 до 1
			const normX = dx / radius;
			const normY = dy / radius;

			// Яркость (0-1) - чем ближе к центру, тем ярче
			const brightness = 1 - (distance / radius);

			// Преобразуем в HSL
			let hue = (angle * 180 / Math.PI + 360) % 360;
			let saturation = Math.sqrt(normX * normX + normY * normY);

			// Ограничиваем насыщенность
			saturation = Math.min(1, saturation);

			// Конвертируем HSL в RGB
			var c = hslToRgb(hue, saturation, 0.5);

			// Обновляем превью
			colorPreview.style.backgroundColor = c;
			colorHex.textContent = c;

			if(e instanceof PointerEvent){
				colorSelected.style.backgroundColor = c;
				selectedColor = c;
			}
		}
	}

	// Функция конвертации HSL в HEX
	function hslToRgb(h, s, l) {
		h /= 360;
		let r, g, b;

		if (s === 0) {
			r = g = b = l;
		} else {
			const hue2rgb = (p, q, t) => {
				if (t < 0) t += 1;
				if (t > 1) t -= 1;
				if (t < 1 / 6) return p + (q - p) * 6 * t;
				if (t < 1 / 2) return q;
				if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
				return p;
			};

			const q = l < 0.5 ? l * (1 + s) : l + s - l * s;
			const p = 2 * l - q;

			r = hue2rgb(p, q, h + 1 / 3);
			g = hue2rgb(p, q, h);
			b = hue2rgb(p, q, h - 1 / 3);
		}

		const toHex = x => {
			const hex = Math.round(x * 255).toString(16);
			return hex.length === 1 ? '0' + hex : hex;
		};

		return `#${toHex(r)}${toHex(g)}${toHex(b)}`.toUpperCase();
	}

	// Обработчики событий
	colorWheel.addEventListener('mousemove', handleColorSelect);
	// colorWheel.addEventListener('mouseup', handleColorSelect);
	colorWheel.addEventListener('click', handleColorSelect);

	// Кнопка отмены
	colorCancelBtn.addEventListener('click', () => {
		colorPickerModal.style.display = 'none';
	});

	// Кнопка подтверждения
	colorConfirmBtn.addEventListener('click', () => {
		colorPickerModal.style.display = 'none';
		onChangeColorClicked();
	});

	colorElements = document.querySelectorAll(".color-me");
	colorElements.forEach(element => {
        const color = intToHexColor(Number(element.textContent.trim())); // Получаем текст ("#ff0000")
        element.style.backgroundColor = color; // Устанавливаем фон
    });
}

// Функция для открытия цветового пикера
function openColorPicker(propId) {
	const colorPickerModal = document.getElementById('colorPickerModal');
	const colorPreview = document.getElementById('colorPreview');
	const colorSelected = document.getElementById('colorSelected');
	const colorHex = document.getElementById('colorHex');
	const colorSelector = document.getElementById('colorSelector');

	currentProperty = parseInt(propId);
	currentColor = intToHexColor(Number(document.getElementById("item-value-" + propId).textContent))

	//convert it from value as integer rgba
	selectedColor = currentColor;

	colorPreview.style.backgroundColor = currentColor;
	colorSelected.style.backgroundColor = currentColor;
	colorHex.textContent = currentColor;

	// Позиционируем селектор (для начального цвета потребуется конвертация RGB->HSL)
	// Здесь упрощенная логика - ставим селектор в центр
	const wheel = document.getElementById('colorWheel');
	const centerX = wheel.offsetWidth / 2;
	const centerY = wheel.offsetHeight / 2;

	colorSelector.style.left = centerX + 'px';
	colorSelector.style.top = centerY + 'px';

	colorPickerModal.style.display = 'flex';
}

// Инициализируем при загрузке
document.addEventListener('DOMContentLoaded', initColorPicker);