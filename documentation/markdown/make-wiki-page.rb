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

pipeline = HTML::Pipeline.new [
  HTML::Pipeline::MarkdownFilter,
  HTML::Pipeline::WikiLinkFilter,
	HTML::Pipeline::RougeFilter
]
result = pipeline.call input

code = result[:output].to_s

if ARGV[1]
	File.write(ARGV[1], code)
else
	puts code
end
