// Элементы модальных окон
const propertyModal = document.getElementById('property-modal');
const loadingOverlay = document.getElementById('loading-overlay');
const propertyValueInput = document.getElementById('property-value');
const errorMessage = document.getElementById('error-message');

var currentProperty = 0;

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
	if(!propertyValueInput.value){
		showError("Не допустимое значение");
		return;
	}
	openLoadingModal()
	// closePropertyModal();
	 // // Создаем AJAX запрос
    const xhr = new XMLHttpRequest();
    xhr.open('POST', "/property/" + currentProperty, true);
	xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	
    
    xhr.onload = function() {
        if (this.status === 200) {
            // Заменяем весь HTML документа на полученный
            // document.documentElement.innerHTML = this.responseText;
            
            // Восстанавливаем обработчики событий (так как весь DOM был заменен)
            // restoreEventListeners();
			window.location.reload();
        } else {
            console.error('Ошибка отправки запроса:', this.statusText);
            // alert('Произошла ошибка при отправке запроса');
			showError("Ошибка изменения");

        }
        
        // Скрываем индикатор загрузки
        closeLoadingModal();
		closePropertyModal();

    };
    
    xhr.onerror = function() {
        console.error('Ошибка запроса');
        alert('Произошла ошибка сети');
        closeLoadingModal();
		closePropertyModal();
    };
    
    xhr.send("value=" + encodeURIComponent(propertyValueInput.value));
} 