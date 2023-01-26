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

function deleteFile() {
	const fileName = document.getElementById("file-input-del").value;
	fetch("delete.php/upload/" + fileName, {
	  method: "DELETE"
	}).catch(error => {
	  console.error("Error:", error);
	});
}
