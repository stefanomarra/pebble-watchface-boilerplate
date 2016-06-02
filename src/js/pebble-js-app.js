Pebble.addEventListener('ready', function() {
	console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
	var url = 'https://rawgit.com/stefanomarra/pebble-watchface-boilerplate/master/config/index.html';

	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
	var configData = JSON.parse(decodeURIComponent(e.response));
	console.log('Configuration page returned: ' + JSON.stringify(configData));

	var dict = {};
	dict.KEY_COLOR_BACKGROUND  = parseInt(configData.background_color, 16);
	dict.KEY_COLOR_TIME        = parseInt(configData.time_text_color, 16);
	dict.KEY_COLOR_DATE        = parseInt(configData.date_text_color, 16);
	dict.KEY_COLOR_HOUR_HAND   = parseInt(configData.hour_hand_color, 16);
	dict.KEY_COLOR_MINUTE_HAND = parseInt(configData.minute_hand_color, 16);
	dict.KEY_COLOR_SECOND_HAND = parseInt(configData.second_hand_color, 16);
	dict.KEY_SHOW_HANDS        = configData.show_hands ? 1 : 0;
	dict.KEY_SHOW_SECOND_HAND  = configData.show_second_hand ? 1 : 0;
	dict.KEY_SHOW_TIME         = configData.show_time ? 1 : 0;
	dict.KEY_SHOW_DATE         = configData.show_date ? 1 : 0;

	console.log('Configuration page returned: ' + JSON.stringify(dict));

	// Send to watchapp
	Pebble.sendAppMessage(dict, function() {
		console.log('Send successful: ' + JSON.stringify(dict));
	}, function(e) {
		console.log('Send failed! Unable to deliver message with transactionId=' + e.data.transactionId + ' Error is: ' + e.error.message);
	});
});