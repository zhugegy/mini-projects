import webapp2
import cgi
import urllib
import os
from google.appengine.ext import blobstore
from google.appengine.ext.webapp import blobstore_handlers
from google.appengine.ext import ndb

class SecondMainPage(webapp2.RequestHandler):

    def get(self):
		upload_url = blobstore.create_upload_url('/message')
		
		self.response.out.write('<html><body>')
		self.response.write('Welcome to second page, please leave a message here.<br />')
		self.response.write('You can also upload a file with your message, but the format is restricted within txt, jpeg, gif, doc, odt, pdf, png, tif, tiff, pps, mp3, wav, avi, mpg, mpeg, wmv, iso, flv, mov, dvr-ms, vob, zip, rar, r00, r01, otherwise it will be deleted.<br />')
		qry = Message.query().order(-Message.message_time)
		self.response.out.write('</body></html>')
		self.response.out.write('<form action="%s" method="POST" enctype="multipart/form-data">' % upload_url)
		self.response.out.write("""From: <input type="text" name="from" /> <br />""")
		self.response.out.write("""Message: <input type="text" name="text" /><br />""")
		self.response.out.write("""Upload File: <input type="file" name="myUploadedFile2"><br />""")
		self.response.out.write("""<input type="submit" value="Submit" name="submit"> </form>""")
		
		for message in qry.fetch(10):
			self.response.out.write('-------------------------------------------<br />')
			self.response.out.write('From: ' + cgi.escape(message.message_from) + '<br />')
			self.response.out.write('Text: ' + cgi.escape(message.message_text) + '<br />')
			self.response.out.write('Time: ' + str(message.message_time) + '<br />')
			self.response.out.write('<li><a href="/serve/%s' % str(message.message_filekey) + '">' + str(message.message_filename) + '</a>\n' + '<br />')
			
		self.response.out.write('-------------------------------------------<br />')
		

class Message(ndb.Model):
	
	message_from = ndb.StringProperty()
	message_text = ndb.StringProperty()
	message_time = ndb.DateTimeProperty(auto_now=True)
	message_filekey = ndb.StringProperty()
	message_filename = ndb.StringProperty()
	
class HandleMessage(blobstore_handlers.BlobstoreUploadHandler):

	def post(self):
		upload_files2 = self.get_uploads('myUploadedFile2')
		
		if upload_files2:
			blob_info2 = upload_files2[0]
			m_filekey = str(blob_info2.key())
			m_filename = str(blob_info2.filename)
			items = m_filename.split('.')
			changdu = len(items)-1
			
			if (items[changdu]=='txt' or items[changdu]=='jpeg' or items[changdu]=='gif' or items[changdu]=='doc' or items[changdu]=='odt' or items[changdu]=='pdf' or items[changdu]=='png' or items[changdu]=='tif' or items[changdu]=='tiff' or items[changdu]=='pps' or items[changdu]=='mp3' or items[changdu]=='wav' or items[changdu]=='avi' or items[changdu]=='mpg' or items[changdu]=='mpeg' or items[changdu]=='wmv' or items[changdu]=='iso' or items[changdu]=='flv' or items[changdu]=='mov' or items[changdu]=='dvr-ms' or items[changdu]=='vob' or items[changdu]=='zip' or items[changdu]=='rar' or items[changdu]=='r00' or items[changdu]=='r01'):
				m_from = self.request.get('from')
				m_text = self.request.get('text')
				message = Message(message_from=m_from, message_text=m_text, message_filekey=m_filekey, message_filename=m_filename)
				message.put()
				self.response.out.write('<html><body>')
				self.response.out.write('Message submit successed! ')
				self.response.out.write('Uploaded file: ' + str(blob_info2.filename))
				self.response.out.write('<li><a href="/serve/%s' % str(blob_info2.key()) + '">' + str(blob_info2.filename) + '</a>')
				self.response.out.write('<li><a href="/secondexercise">BackToPreviousPage</a>')
				self.response.out.write('</body></html>')
				
			else:
				m_from = self.request.get('from')
				m_text = self.request.get('text')
				message = Message(message_from=m_from, message_text=m_text, message_filekey='None', message_filename='DELETED')
				message.put()
				self.response.out.write('<html><body>')
				self.response.out.write('Sorry! Your file has been deleted because of format restriction. ')
				self.response.out.write('Uploaded file: DELETED')
				self.response.out.write('<li><a href="/serve/None">DELETED</a>')
				self.response.out.write('<li><a href="/secondexercise">BackToPreviousPage</a>')
				self.response.out.write('</body></html>')
				blob_info2.delete()
				
		else:	
			m_from = self.request.get('from')
			m_text = self.request.get('text')
			message = Message(message_from=m_from, message_text=m_text)
			message.put()
			self.response.out.write('<html><body>')
			self.response.out.write('Message submit successed! ')
			self.response.out.write('Uploaded file: None')
			self.response.out.write('<li><a href="/secondexercise">BackToPreviousPage</a>')
			self.response.out.write('</body></html>')
			
class ServeFile2(blobstore_handlers.BlobstoreDownloadHandler):
	def get(self, resource):
		resource = str(urllib.unquote(resource))
		blob_info2 = blobstore.BlobInfo.get(resource)
		self.send_blob(blob_info2)