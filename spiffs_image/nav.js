function loadPage(pageUrl) {
    // Показываем индикатор загрузки
    // document.getElementById('loading-overlay').style.display = 'flex';
    window.location.href = window.location.protocol + '//' + 
		window.location.hostname + pageUrl;
}

// Восстанавливаем обработчики событий после замены DOM
function restoreEventListeners() {
    // Находим все кнопки с классом btn и проверяем их атрибут onclick
    const buttons = document.querySelectorAll('.list-item');
    buttons.forEach(button => {
        const onClick = button.getAttribute('onclick');
        if (onClick && onClick.startsWith('loadPage')) {
            // Извлекаем URL из строки onclick
            const url = onClick.match(/loadPage\('(.+?)'\)/)[1];
            button.onclick = function() { loadPage(url); };
        }
    });
}

// Инициализация при первой загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    restoreEventListeners();
});