class MenusController < ApplicationController
  def show
    lat = params[:lat].to_f
    lon = params[:lon].to_f
    min_stops = (params[:min_stops] || 4).to_i
    stops_and_routes = Stop.as_pebble_menu(lat, lon, min_stops)

    render json: stops_and_routes.to_json
  end
end
