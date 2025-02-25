addEventListener("load", (event) => {
});



var request_in_progress = false;



function _post_property(id, val) {
	return fetch(`/property/${id}`, {
		method: "POST",
		headers: {
			"Content-type": "application/x-www-form-urlencoded"
		},
		body: `value=${val}`
	});
}

function _show_wait_dialog() {
	document.getElementById("wait_dialog").style.display = "block"
	document.getElementById("controls").style.display = "none"
}

function _close_wait_dialog() {
	document.getElementById("wait_dialog").style.display = "none"
	document.getElementById("controls").style.display = "grid"
}

async function _post_with_dialog(id, val) {
	_show_wait_dialog();

	const response = await _post_property(id, val);
	if (await response.status != 200) {
		console.log("failed response, status: " + response.status)
	}

	_close_wait_dialog();
}

async function change_color(event) {
	if (request_in_progress) {
		return;
	}

	request_in_progress = true;

	var rgb_color = parseInt(document.getElementById("color").value.substr(1), 16);
	var white_color = parseInt(document.getElementById("white_color").value, 10);
	var color = (white_color << 24) + rgb_color
	
	var id = document.getElementById("id_color").value;

	if (event.type == "click") {
		_post_with_dialog(id, color);
	} else {
		const response = await _post_property(id, color);
		await response.status;
	}

	request_in_progress = false;
}

async function turn_on() {
	var id = document.getElementById("id_switch").value;

	_post_with_dialog(id, 1);
}

async function turn_off() {
	var id = document.getElementById("id_switch").value;

	_post_with_dialog(id, 2);
}

async function play_track() {
	var id = document.getElementById("id_play").value;
	var track_id = document.getElementById("track_id").value;

	_post_with_dialog(id, track_id);
}

async function stop_audio() {
	var id = document.getElementById("id_stop").value;

	_post_with_dialog(id, "");
}

async function set_volume() {
	var id = document.getElementById("id_volume").value;
	var volume = document.getElementById("volume").value;

	_post_with_dialog(id, volume);
}