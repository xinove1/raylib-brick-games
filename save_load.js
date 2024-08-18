addToLibrary({
	save_data: function(where_cstr, ptr, size) {
		const where = UTF8ToString(where_cstr);
		const data = HEAPU8.subarray(ptr, ptr + size);
		localStorage.setItem(where, JSON.stringify(data));
	},
	load_data: function(where_cstr, dst, size) {
		const where = UTF8ToString(where_cstr);
		const data = Object.values(JSON.parse(localStorage.getItem(where)));
		let i = 0;
		for (let byte of data) {
			HEAPU8[dst + i] = byte;
			i++;
		}
	},
});
