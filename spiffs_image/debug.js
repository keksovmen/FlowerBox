// ===== Configuration =====
const WS_URL = `ws://${location.host}:80/debug/live`;
const RECONNECT_DELAYS = [1000, 2000, 4000, 8000, 16000, 30000];
const MAX_MESSAGES = 10000;

// ===== State =====
let socket = null;
let reconnectAttempt = 0;
let reconnectTimer = null;
let buffer = '';
let scrollEnable = false;
let manualDisconnect = false;
let userScrolled = false;

// ===== DOM refs =====
const contentDiv = document.getElementById('content');
const statusDot = document.getElementById('statusDot');
const statusText = document.getElementById('statusText');
const btnReconnect = document.getElementById('btnReconnect');
const btnDisconnect = document.getElementById('btnDisconnect');
const btnScroll = document.getElementById('btnScroll');

// ===== UI update =====
function setStatus(connected) {
	if (connected) {
		statusDot.className = 'status-dot connected';
		statusText.textContent = 'Connected';
	} else {
		statusDot.className = 'status-dot disconnected';
		statusText.textContent = 'Disconnected';
	}
}

// ===== Helper: decode HTML entities =====
function decodeHtmlEntities(html) {
	const txt = document.createElement('textarea');
	txt.innerHTML = html;
	return txt.value;
}

// ===== Update scroll button text based on current state =====
function updateScrollButton() {
	// Auto-scroll is effectively active if both conditions are true
	const isActive = scrollEnable && !userScrolled;
	btnScroll.textContent = isActive ? 'Disable scroll' : 'Enable scroll';
}

// ===== Message rendering =====
function renderMessage(type, message) {
	const decoded = decodeHtmlEntities(message);
	const div = document.createElement('div');
	div.className = `log-message log-${type}`;
	div.textContent = decoded;
	contentDiv.appendChild(div);

	// Enforce message limit – remove oldest if over limit
	while (contentDiv.children.length > MAX_MESSAGES) {
		contentDiv.removeChild(contentDiv.firstChild);
	}

	// Auto-scroll only if enabled and user hasn't manually scrolled up
	if (scrollEnable && !userScrolled) {
		contentDiv.scrollTop = contentDiv.scrollHeight;
	}
}

// ===== Buffer processing =====
function processBuffer() {
	while (true) {
		const start = buffer.indexOf('{{');
		if (start === -1) break;

		const end = buffer.indexOf('}}', start + 2);
		if (end === -1) break;

		const inner = buffer.substring(start + 2, end);
		buffer = buffer.substring(end + 2);

		if (inner.startsWith('l=') && inner.length >= 4) {
			const typeChar = inner.charAt(2);
			const msg = inner.substring(4);
			renderMessage(typeChar, msg);
		} else {
			renderMessage('plain', inner);
		}
	}
}

// ===== WebSocket handling =====
function connectWebSocket() {
	if (socket && socket.readyState === WebSocket.OPEN) return;

	if (socket) {
		socket.close();
		socket = null;
	}

	manualDisconnect = false;
	setStatus(false);

	try {
		socket = new WebSocket(WS_URL);

		socket.onopen = function () {
			console.log('WebSocket connected');
			setStatus(true);
			reconnectAttempt = 0;
			if (reconnectTimer) {
				clearTimeout(reconnectTimer);
				reconnectTimer = null;
			}
		};

		socket.onmessage = function (event) {
			buffer += event.data;
			processBuffer();
		};

		socket.onclose = function (event) {
			console.log('WebSocket closed', event);
			setStatus(false);
			if (!manualDisconnect) {
				scheduleReconnect();
			}
		};

		socket.onerror = function (error) {
			console.error('WebSocket error:', error);
			setStatus(false);
		};
	} catch (e) {
		console.error('Failed to create WebSocket:', e);
		setStatus(false);
		if (!manualDisconnect) {
			scheduleReconnect();
		}
	}
}

function scheduleReconnect() {
	if (manualDisconnect) return;

	if (reconnectTimer) {
		clearTimeout(reconnectTimer);
		reconnectTimer = null;
	}

	const delay = reconnectAttempt < RECONNECT_DELAYS.length
		? RECONNECT_DELAYS[reconnectAttempt]
		: RECONNECT_DELAYS[RECONNECT_DELAYS.length - 1];

	reconnectAttempt++;
	console.log(`Reconnecting in ${delay}ms (attempt ${reconnectAttempt})`);

	reconnectTimer = setTimeout(() => {
		if (!socket || socket.readyState === WebSocket.CLOSED || socket.readyState === WebSocket.CLOSING) {
			connectWebSocket();
		} else {
			setStatus(true);
		}
		reconnectTimer = null;
	}, delay);
}

function disconnectWebSocket() {
	manualDisconnect = true;
	if (reconnectTimer) {
		clearTimeout(reconnectTimer);
		reconnectTimer = null;
	}
	if (socket) {
		socket.close();
		socket = null;
	}
	setStatus(false);
	reconnectAttempt = 0;
}

// ===== Button: toggle auto-scroll =====
function toggleScroll() {
	const isActive = scrollEnable && !userScrolled;
	if (isActive) {
		// Currently enabled → disable
		scrollEnable = false;
	} else {
		// Currently disabled → enable, reset user scroll, and jump to bottom
		scrollEnable = true;
		userScrolled = false;
		contentDiv.scrollTop = contentDiv.scrollHeight;
	}
	updateScrollButton();
}

// ===== Scroll event – detect manual interaction =====
contentDiv.addEventListener('scroll', function () {
	const atBottom = contentDiv.scrollHeight - contentDiv.clientHeight <= contentDiv.scrollTop + 1;
	userScrolled = !atBottom;
	// Update button text only if state changed (to avoid extra DOM writes)
	updateScrollButton();
});

// ===== Init =====
function init() {
	setStatus(false);
	connectWebSocket();

	btnReconnect.addEventListener('click', function () {
		manualDisconnect = false;
		if (reconnectTimer) {
			clearTimeout(reconnectTimer);
			reconnectTimer = null;
		}
		if (socket) {
			socket.close();
			socket = null;
		}
		connectWebSocket();
	});

	btnDisconnect.addEventListener('click', function () {
		disconnectWebSocket();
	});

	btnScroll.addEventListener('click', toggleScroll);

	// Initial button text
	updateScrollButton();

	window.addEventListener('beforeunload', function () {
		if (socket) socket.close();
		if (reconnectTimer) clearTimeout(reconnectTimer);
	});

	// Back navigation button
	document.getElementById('btnBack').addEventListener('click', function () {
		window.history.back();
	});
}

if (document.readyState === 'loading') {
	document.addEventListener('DOMContentLoaded', init);
} else {
	init();
}