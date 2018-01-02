from os.path import join as path_join
from conans import ConanFile, tools
import zipfile

class LightningcppConan(ConanFile):
	name = "lightningcpp"
	version = 'master'
	url = 'https://github.com/'
	license = 'LGPL'

	def source(self):
		tools.download("https://github.com/lightningcpp/lightningcpp/archive/master.zip".format(self.version), "lightningcpp.zip")
		zip_ref = zipfile.ZipFile("lightningcpp.zip", 'r')
		zip_ref.extractall()
		zip_ref.close()

	def package(self):
		self.copy('*.h', dst='include', keep_path=False)

	def build(self):
		pass

