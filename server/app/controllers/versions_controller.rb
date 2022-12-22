class VersionsController < ApplicationController
  def show
    render html: Git.open(Rails.root.parent).log.first.sha
  end
end
