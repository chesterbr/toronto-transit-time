class MenusController < ApplicationController
  def show
    lat, lon = params[:lat].to_f, params[:lon].to_f
    stops_and_routes = Stop.as_pebble_menu(lat,lon)

    render json: stops_and_routes.to_json
  end
end
