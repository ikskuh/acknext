<!doctype html!>
<html>
<head>
	<meta charset="UTF-8" />
	<title>Acknext Documentation</title>
	<style type="text/css">
		#content {
			position: fixed;
			left: 256px;
			width: calc(100% - 256px);
			top: 0;
			height: 100%;
			border: none;
			padding-left: 1em;
		}
		#toc {
			position: fixed;
			left: 0px;
			width: 256px;
			top: 0;
			height: 100%;
			overflow: scroll;
			border: none;
			padding-left: 1em;
		}
		ul {
			list-style-image: url('folder.png');
		}
		ul {
			padding-left: 1.5em;
		}
	</style>
	<script type="text/javascript">
		function toggleDiv(div) {
			div.style.display = div.style.display == "none" ? "block" : "none";
		}
		function navClick(ev) {
			toggleDiv(ev.path[0].nextElementSibling);
		}
	</script>
</head>
<body>
	<div id="toc">
		<?php echo file_get_contents("toc.htm") ?>
	</div>
	<iframe id="content" src="view_create.htm">
		
	</iframe>
	
	<script type="text/javascript">
		var content = document.getElementById("content")
		function load(x) { content.src = x + ".htm"; }
		
		var a = document.getElementsByClassName("treenode");
		for(i = 0; i < a.length; i++) {
			a[i].onclick = navClick;
		}
		window.addEventListener('message', function(event) {
      switch(event.data.type) {
				case "load":
					load(event.data.what);
					break;
			}
    }, false);
	</script>
</body>

</html>