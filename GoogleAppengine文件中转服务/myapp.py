from google.appengine.api import users
from google.appengine.ext.webapp import blobstore_handlers
from google.appengine.ext import blobstore
from second import *
import os
import urllib

import webapp2


class MainPage(webapp2.RequestHandler):

	def get(self):
		user = users.get_current_user()
		upload_url = blobstore.create_upload_url('/upload')
		
		if user:
			self.response.out.write('<html><body>')
			self.response.write('Welcome to system, ' + user.nickname() + '.')
			self.response.out.write('</body></html>')
			self.response.out.write('<form action="%s" method="POST" enctype="multipart/form-data">' % upload_url)
			self.response.out.write("""Upload File: <input type="file" name="myUploadedFile"><br />""")
			self.response.out.write("""<input type="submit" name="submit" value="Submit"> </form>""")
			for blob_info in blobstore.BlobInfo.all():
				if str(blob_info.filename)!='':
					self.response.out.write('<li><a href="/serve/%s' % str(blob_info.key()) + '">' + str(blob_info.filename) + '</a>\n')
		else:
			self.redirect(users.create_login_url(self.request.uri))

class Upload(blobstore_handlers.BlobstoreUploadHandler):

	def post(self):
		upload_files = self.get_uploads('myUploadedFile')
		
		if upload_files:
			blob_info = upload_files[0]
			self.response.out.write('<html><body>')
			self.response.out.write('Uploaded file: ' + str(blob_info.filename))
			self.response.out.write('<li><a href="/serve/%s' % str(blob_info.key()) + '">' + str(blob_info.filename) + '</a>')
			self.response.out.write('</body></html>')
		else:
			self.response.out.write('<html><body>')
			self.response.out.write('None file is selected, please go back to previous page and select a file.')
			self.response.out.write('<li><a href="/">Back</a>')
			self.response.out.write('</body></html>')

class ServeFile(blobstore_handlers.BlobstoreDownloadHandler):
	def get(self, resource):
		resource = str(urllib.unquote(resource))
		blob_info = blobstore.BlobInfo.get(resource)
		self.send_blob(blob_info)

application = webapp2.WSGIApplication([
    ('/', MainPage),
	('/upload', Upload),
	('/serve/([^/]+)?', ServeFile),
	('/serve/([^/]+)?', ServeFile2),
	('/secondexercise', SecondMainPage),
	('/message', HandleMessage),
], debug=True)

