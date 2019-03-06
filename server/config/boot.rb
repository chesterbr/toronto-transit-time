ENV['BUNDLE_GEMFILE'] ||= File.expand_path('../Gemfile', __dir__)

require 'bundler/setup' # Set up gems listed in the Gemfile.
# Disabled this because this app is tiny
# require 'bootsnap/setup' # Speed up boot time by caching expensive operations.
