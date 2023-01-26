function uploadFile() {
	const fileInput = document.getElementById("file-input");
	const file = fileInput.files[0];
	fetch(file.name, {
	  method: "POST",
	  body: file
	}).catch(error => {
	  console.error("Error:", error);
	});
}
