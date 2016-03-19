class CreateStops < ActiveRecord::Migration[5.0]
  def change
    create_table :stops do |t|
      t.string :tag, index: true
      t.string :title
      t.float :lat
      t.float :lon
    end
  end
end
