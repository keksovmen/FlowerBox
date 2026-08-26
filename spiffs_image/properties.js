// ===== State =====
let currentProperty = {
	id: null,
	value: null,
	min: null,
	max: null,
	type: null
};
let isSending = false;
let pendingNoneAction = null; // { propId }

// ===== DOM refs =====
const modal = document.getElementById('property-modal');
const modalTitle = document.getElementById('modal-title');
const modalDesc = document.getElementById('modal-desc');
const modalRange = document.getElementById('modal-range');
const modalInput = document.getElementById('property-value-input');
const modalError = document.getElementById('modal-error');
const modalSendBtn = document.getElementById('modal-send-btn');
const modalLoading = document.getElementById('modal-loading');
const toastContainer = document.getElementById('toast-container');
const confirmModal = document.getElementById('confirm-modal');
const confirmTitle = document.getElementById('confirm-title');
const confirmMessage = document.getElementById('confirm-message');

// ===== Helper: POST to property endpoint =====
async function postProperty(id, value) {
	const response = await fetch(`/property/${id}`, {
		method: "POST",
		headers: {
			"Content-Type": "application/x-www-form-urlencoded"
		},
		body: `value=${value}`
	});
	return response;
}

// ===== Show toast notification =====
function showToast(message, isSuccess) {
	const toast = document.createElement('div');
	toast.className = `toast ${isSuccess ? 'toast-success' : 'toast-error'}`;
	toast.textContent = message;
	toastContainer.appendChild(toast);
	setTimeout(() => {
		toast.style.opacity = '0';
		setTimeout(() => toast.remove(), 300);
	}, 3000);
}

// ===== Update value in DOM =====
function updateValueDisplay(propId, newValue) {
	const valueSpan = document.getElementById(`value-${propId}`);
	if (valueSpan) {
		valueSpan.textContent = newValue;
	}
	// Update data-current-value on the mode-btn-group if it exists
	const group = document.querySelector(`.mode-btn-group[data-prop-id="${propId}"]`);
	if (group) {
		group.dataset.currentValue = newValue;
		highlightActiveButtons();
	}
}

// ===== Highlight the active mode button =====
function highlightActiveButtons() {
	document.querySelectorAll('.mode-btn-group').forEach(group => {
		const currentVal = parseInt(group.dataset.currentValue);
		group.querySelectorAll('.mode-btn').forEach(btn => {
			const btnState = parseInt(btn.dataset.state);
			btn.classList.toggle('active', btnState === currentVal);
		});
	});
}

// ===== Open modal for editing (normal property) =====
function openPropertyModal(element) {
	const id = parseInt(element.dataset.id);
	const type = element.dataset.type;

	// Special handling for ID 514 (mode buttons) – do nothing
	if (id === 514) return;

	// If type is "none", show confirmation dialog (no input)
	if (type === 'none') {
		const name = element.querySelector('.item-name')?.textContent || 'Свойство';
		const desc = element.querySelector('.item-description')?.textContent || '';
		confirmTitle.textContent = `Выполнить: ${name}`;
		confirmMessage.textContent = desc || 'Подтвердить действие?';
		pendingNoneAction = { propId: id };
		confirmModal.style.display = 'flex';
		return;
	}

	// Otherwise, show input modal with validation
	const value = element.dataset.value;
	const min = element.dataset.min !== "" ? parseFloat(element.dataset.min) : null;
	const max = element.dataset.max !== "" ? parseFloat(element.dataset.max) : null;

	currentProperty.id = id;
	currentProperty.value = value;
	currentProperty.min = min;
	currentProperty.max = max;
	currentProperty.type = type;

	const name = element.querySelector('.item-name')?.textContent || 'Свойство';
	modalTitle.textContent = `Изменить: ${name}`;
	modalDesc.textContent = `Текущее значение: ${value}`;

	if (min !== null && max !== null && (type === 'int' || type === 'float')) {
		modalRange.style.display = 'block';
		modalRange.textContent = `Диапазон: ${min} … ${max}`;
	} else {
		modalRange.style.display = 'none';
	}

	modalInput.value = value;
	modalError.style.display = 'none';
	modalSendBtn.disabled = false;
	document.getElementById('modal-loading-overlay').style.display = 'none';

	modal.style.display = 'flex';
	modalInput.focus();
}

// ===== Close input modal =====
function closeModal() {
	modal.style.display = 'none';
	currentProperty.id = null;
	isSending = false;
}

function closeModalOutside(event) {
	if (event.target === modal) {
		closeModal();
	}
}

// ===== Send property change (input modal) =====
async function sendProperty() {
	if (isSending) return;
	const newValue = modalInput.value.trim();
	if (newValue === '') {
		showToast('Введите значение', false);
		return;
	}

	const type = currentProperty.type;
	let finalValue = newValue;
	if (type === 'int' || type === 'float') {
		const num = parseFloat(newValue);
		if (isNaN(num)) {
			showToast('Введите число', false);
			return;
		}
		if (currentProperty.min !== null && num < currentProperty.min) {
			showToast(`Значение не может быть меньше ${currentProperty.min}`, false);
			return;
		}
		if (currentProperty.max !== null && num > currentProperty.max) {
			showToast(`Значение не может быть больше ${currentProperty.max}`, false);
			return;
		}
		finalValue = num;
	}

	isSending = true;
	modalSendBtn.disabled = true;
	document.getElementById('modal-loading-overlay').style.display = 'flex';
	modalError.style.display = 'none';

	try {
		const response = await postProperty(currentProperty.id, finalValue);
		if (response.status === 200) {
			showToast('Свойство обновлено', true);
			updateValueDisplay(currentProperty.id, finalValue);
			closeModal();
		} else {
			const errMsg = await response.text();
			showToast(`Ошибка: ${errMsg || response.status}`, false);
			modalError.textContent = `Ошибка: ${errMsg || response.status}`;
			modalError.style.display = 'block';
		}
	} catch (e) {
		console.error(e);
		showToast('Ошибка соединения', false);
		modalError.textContent = 'Ошибка соединения';
		modalError.style.display = 'block';
	} finally {
		isSending = false;
		modalSendBtn.disabled = false;
		document.getElementById('modal-loading-overlay').style.display = 'none';
	}
}

// ===== Confirmation modal for "none" type =====
function closeConfirmModal() {
	confirmModal.style.display = 'none';
	pendingNoneAction = null;
}

function closeConfirmModalOutside(event) {
	if (event.target === confirmModal) {
		closeConfirmModal();
	}
}

// ===== Execute action for "none" type (send value 1) =====
async function executeNoneAction() {
	if (!pendingNoneAction) return;
	const propId = pendingNoneAction.propId;
	closeConfirmModal();

	// Show a brief loading indicator on the button? We'll just disable the button temporarily
	const confirmBtn = document.getElementById('confirm-btn');
	confirmBtn.disabled = true;
	confirmBtn.textContent = '...';

	try {
		// We send 1 as a trigger (you can change this if needed)
		const response = await postProperty(propId, 1);
		if (response.status === 200) {
			showToast('Действие выполнено', true);
			// Update the value display (if it changed)
			// The server might return a new value, but we don't have it. We can reload the page or just leave as is.
			// We'll just leave the current value as is, but we could refresh the value from the server if needed.
			// For simplicity, we'll reload the page to reflect any changes.
			// window.location.reload();
		} else {
			const errMsg = await response.text();
			showToast(`Ошибка: ${errMsg || response.status}`, false);
		}
	} catch (e) {
		console.error(e);
		showToast('Ошибка соединения', false);
	} finally {
		confirmBtn.disabled = false;
		confirmBtn.textContent = 'Подтвердить';
	}
}

// ===== Special mode buttons (ID 514) – immediate, no confirmation =====
async function setMode(propId, state) {
	const group = document.querySelector(`.mode-btn-group[data-prop-id="${propId}"]`);
	const btns = group ? group.querySelectorAll('.mode-btn') : [];
	btns.forEach(btn => btn.disabled = true);

	try {
		const response = await postProperty(propId, state);
		if (response.status === 200) {
			updateValueDisplay(propId, state);
			showToast('Режим обновлён', true);
		} else {
			const errMsg = await response.text();
			showToast(`Ошибка: ${errMsg || response.status}`, false);
		}
	} catch (e) {
		console.error(e);
		showToast('Ошибка соединения', false);
	} finally {
		btns.forEach(btn => btn.disabled = false);
	}
}

// ===== Initialize =====
document.addEventListener('DOMContentLoaded', function() {
	highlightActiveButtons();
});