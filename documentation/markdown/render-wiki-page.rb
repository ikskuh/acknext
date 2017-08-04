#!/usr/bin/ruby
require 'github/markup'
require 'html/pipeline'
require 'html/pipeline/wiki_link'
require 'html/pipeline/rouge_filter'


if ARGV[0]
	input = File.read(ARGV[0])
else
	input = STDIN.read
end

context = {
  :base_url   => "./index.htm?page="
}

pipeline = HTML::Pipeline.new [
  HTML::Pipeline::MarkdownFilter,
#  HTML::Pipeline::WikiLinkFilter,
	HTML::Pipeline::RougeFilter
], context

result = pipeline.call input

code = result[:output].to_s

puts <<HTML
<!doctype html!>
<html>
<head>
<link rel="stylesheet" type="text/css" href="page-style.css">
</head>
<body>
HTML

puts code

puts <<HTML
<script type="text/javascript">
var content = window.frameElement.getElementById("content")
function load(x) { 
	window.parent.postMessage({
		type: 'load',
		what: x
	}, '*');
}
</script>
</body>
</html>
HTML