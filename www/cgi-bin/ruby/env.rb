#!/usr/bin/ruby

puts "Content-type: text/html\n"
puts "\n"
puts "<html>"
puts "<head><title>Environment Variables</title></head>"
puts "<body>"
puts "<h1>Environment Variables</h1>"
puts "<ul>"
ENV.each do |key, value|
  puts "<li><strong>#{key}:</strong> #{value}</li>"
end
puts "</ul>"
puts "</body>"
puts "</html>"
