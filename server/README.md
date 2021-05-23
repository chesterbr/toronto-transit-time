# Toronto Transit Time Server

This app gathers the TTC stops from the TTC API for [Toronto Transit Time](https://github.com/chesterbr/toronto-transit-time) Pebble app and makes them available in a Pebble-friendly format.

### Setup

Currently, you bootstrap as a traditional Rails app:

- Install the ruby version pointed by [`.ruby-version`](.ruby-version]) (e.g., `rbenv install`);
- Install bundler & dependencies (`gem install bundler && bundle install`);
- Run migrations for the local SQLite database (`rails db:schema:load`);
- Import some stops from the TTC server (`bin/rake ttc:crawl`);
- Run the server (`rails server`);
- Call the `/menu` endpoint to retrieve stops (e.g., [ stops near Union Station](http://localhost:3000/menu?lat=43.6452&lon=-79.3808`));
- Run tests (`bin/rake test`).

### Docker

- This is undergoing migration to Docker (so I can keep this service on with minimal cost/hassle). I'm learning this, so any input on things I'm doing wrong is welcome.
