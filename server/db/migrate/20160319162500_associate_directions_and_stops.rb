class AssociateDirectionsAndStops < ActiveRecord::Migration[5.0]
  def change
    create_table :directions_stops, id: false do |t|
      t.belongs_to :direction, index: true
      t.belongs_to :stop, index: true
    end
  end
end
