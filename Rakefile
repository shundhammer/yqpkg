#--
# Copyright (c) 2015-2021 SUSE LLC
#
# make continuous integration using rubygem-libyui-rake.
#
# MIT license
#++
#
# Common tasks for libyui:
#
#   rake version:bump
#   rake so_version:bump

require "libyui/rake"

Libyui::Tasks.configuration do |conf|
  include Libyui::Tasks::Helpers

  conf.skip_license_check << /.*/
end
