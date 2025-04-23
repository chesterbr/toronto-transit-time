# Toronto Transit Time Server

This app gathers the TTC stops from the TTC API for [Toronto Transit Time](https://github.com/chesterbr/toronto-transit-time) Pebble app and makes them available in a Pebble-friendly format.

### Setup

If you have [rbenv with ruby-build](https://github.com/rbenv/ruby-build) and `wget` (such as in GitHub Codespaces), just :

```bash
rbenv install && bundle && rails db:schema:load && bin/rake ttc:crawl
```

in order to set up the app, its database and import the TTC stops. Then you can run the server (`rails server`) and:
  - Call the `/menu` endpoint to retrieve stops near a specific place with the info needed to display in the list and query for next arrivals (e.g., [ stops near Union Station](http://localhost:3000/menu?lat=43.6452&lon=-79.3808));
  - Run tests (`bin/rake test`).

### TODO

- Add the initialization above to VSCode configuration
- Introduce a Dockerfile
