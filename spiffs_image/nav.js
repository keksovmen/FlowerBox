function loadPage(pageUrl) {
    // Показываем индикатор загрузки
    document.getElementById('loading-overlay').style.display = 'flex';
    window.location.href = pageUrl;
    // // Создаем AJAX запрос
    // const xhr = new XMLHttpRequest();
    // xhr.open('GET', pageUrl, true);
    
    // xhr.onload = function() {
    //     if (this.status === 200) {
    //         // Заменяем весь HTML документа на полученный
    //         document.documentElement.innerHTML = this.responseText;
            
    //         // Восстанавливаем обработчики событий (так как весь DOM был заменен)
    //         restoreEventListeners();
    //     } else {
    //         console.error('Ошибка загрузки страницы:', this.statusText);
    //         alert('Произошла ошибка при загрузке страницы');
    //     }
        
    //     // Скрываем индикатор загрузки
    //     document.getElementById('loading-overlay').style.display = 'none';
    // };
    
    // xhr.onerror = function() {
    //     console.error('Ошибка запроса');
    //     alert('Произошла ошибка сети');
    //     document.getElementById('loading-overlay').style.display = 'none';
    // };
    
    // xhr.send();
}

// Восстанавливаем обработчики событий после замены DOM
function restoreEventListeners() {
    // Находим все кнопки с классом btn и проверяем их атрибут onclick
    const buttons = document.querySelectorAll('.btn');
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