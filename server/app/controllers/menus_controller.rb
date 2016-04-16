class MenusController < ApplicationController
  def show
    lat = params[:lat].to_f
    lon = params[:lon].to_f
    stops_count = (params[:stops_count] || 8).to_i
    stops_and_routes = Stop.as_pebble_menu(lat, lon, stops_count)

    render json: stops_and_routes.to_json
  end
end
