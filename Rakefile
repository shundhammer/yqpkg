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

  conf.skip_license_check << /_multibuild/
  conf.skip_license_check << /^scripts\//
  conf.skip_license_check << /^build-all$/
  conf.skip_license_check << /^Dockerfile$/
  conf.skip_license_check << /legacy-buildtools\/.*/
  conf.skip_license_check << /Makefile\.repo$/
  conf.skip_license_check << /CMakeLists\.txt$/
  conf.skip_license_check << /src\/.*\.h\.in$/
  conf.skip_license_check << /src\/icons\/.*\.svg$/
  conf.skip_license_check << /\/pkgconfig\/.*\.pc\.in$/
  conf.skip_license_check << /^libyui-bindings\/.*/
  conf.skip_license_check << /^libyui-qt\/src\/lang_fonts/
end
