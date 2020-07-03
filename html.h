const char PAGE_NetworkConfiguration[] PROGMEM = R"=====( 
<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<title >ESP kết nối WIFI</title>
</head>
<body>
	<h1 style ="color:red; font-size: 40px;">Getting the password</h1>
	<form action="caidat">
		<label style ="color:blue; font-size:30px ;" for="ten"> Tên WiFi:</label> 
		<input style ="font-size: 20px;"  type="text" id="ten"  name="ten"> <br><br>
		<label style="color: blue; font-size: 30px;" for="matkhau">Mật khẩu:</label>
		<input style ="font-size: 20px;"  type="text" id="matkhau"  name="matkhau"> <br><br>
		<input style="color: green; font-size: 20px; " type="submit">
	</form>

</body>
</html>
)=====";
