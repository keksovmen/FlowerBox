addEventListener("load", (event) => {
});



var request_in_progress = false;



function _post_property(id, val)
{
	return fetch(`/property/${id}`, {
		method: "POST",
		headers: {
			"Content-type": "application/x-www-form-urlencoded"
		},
		body: `value=${val}`
	});
}

async function change_color()
{
	if(request_in_progress){
		return;
	}

	request_in_progress = true;
	
	var color = parseInt(document.getElementById("color").value.substr(1), 16);
	var id = document.getElementById("id_color").value;
	
	const response = await _post_property(id, color);
	
	await response;

	request_in_progress = false;
}

async function turn_on()
{
	var id = document.getElementById("id_switch").value;

	const response = await _post_property(id, 1);

	alert(await response.status == 200 ? "OK" : "FAIL");
}

async function turn_off()
{
	var id = document.getElementById("id_switch").value;

	const response = await _post_property(id, 2);

	alert(await response.status == 200 ? "OK" : "FAIL");
}

async function play_track()
{
	var id = document.getElementById("id_play").value;
	var track_id = document.getElementById("track_id").value;

	const response = await _post_property(id, track_id);

	alert(await response.status == 200 ? "OK" : "FAIL");
}

async function stop_audio()
{
	var id = document.getElementById("id_stop").value;

	const response = await _post_property(id, "");

	alert(await response.status == 200 ? "OK" : "FAIL");
}

async function set_volume()
{
	var id = document.getElementById("id_volume").value;
	var volume = document.getElementById("volume").value;

	const response = await _post_property(id, volume);

	alert(await response.status == 200 ? "OK" : "FAIL");
}