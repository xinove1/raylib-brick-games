addToLibrary({
	save_scores: function(ptr, size, padding) {
		// console.log("ptr: ", ptr, " size: ", size, " padding: ", padding);
		// const number = HEAPU8[ptr + 3];
		// const number2 = HEAPU8[ptr + padding];
		// console.log("first number: ", number);
		// console.log("second number: ", number2);

		const element_count = size / padding;
		const struct = HEAPF32.subarray(ptr/padding, ptr/padding + element_count);
		console.log("elemnt_count: ", element_count);
		console.log("struct: ", struct);
		localStorage.setItem('scores', JSON.stringify(struct));
	},
	load_scores: function(dst, size, padding) {
		const element_count = size / padding;
		const scores = Object.values(JSON.parse(localStorage.getItem('scores')));
		console.log("scores: ", scores);
		console.log("scores.length: ", scores.length);
		console.log("elemnt_count: ", element_count);
		if (element_count < scores.length) {
			console.log("WARNING: load_scores(js): size of dst is smaller than amount of scores saved previously");;
		}
		let i = 0;
		for (let score of scores) {
			console.log(score);
			HEAPF32[dst/padding + i] = score;
			i++;
		}
	},
	save_data: function(where_cstr, ptr, size) {
		const where = UTF8ToString(where_cstr);
		console.log("where: ", where);
		console.log("typeof(where): ", typeof(where));

		const data = HEAPU8.subarray(ptr, ptr + size);
		console.log("data: ", data);
		console.log("typeof(data): ", typeof(data));
		localStorage.setItem(where, JSON.stringify(data));
	},
	load_data: function(where_cstr, dst, size) {
		const where = UTF8ToString(where_cstr);
		const data = Object.values(JSON.parse(localStorage.getItem(where)));
		console.log("data: ", data);
		console.log("typeof(data): ", typeof(data));
		console.log("data.length: ", data.length);
		// if (element_count < scores.length) {
		// 	console.log("WARNING: load_scores(js): size of dst is smaller than amount of scores saved previously");;
		// }
		console.log("before loop");
		let i = 0;
		for (let byte of data) {
			console.log(byte);
			HEAPU8[dst + i] = byte;
			i++;
		}
	},
});
