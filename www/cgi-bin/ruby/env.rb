#!/usr/bin/ruby
puts "HTTP/1.1 200 OK\r\n";
puts "Content-type: text/html\r\n\r\n"
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
