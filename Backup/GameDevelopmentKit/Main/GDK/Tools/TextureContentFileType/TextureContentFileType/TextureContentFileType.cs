using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

using PaintDotNet;
using PaintDotNet.Threading;
using System.IO;

namespace TextureContentFileType
{
    public class TextureContentFileTypes
      : IFileTypeFactory
    {
        public static readonly FileType texture = new TextureContentFileType();

        private static FileType[] fileTypes = new FileType[] { texture };

        public FileType[] GetFileTypeInstances()
        {
            return (FileType[])fileTypes.Clone();
        }
    }

    public class TextureContentFileType : FileType
    {
        public TextureContentFileType()
            : base("GDK Texture", FileTypeFlags.SupportsLoading | FileTypeFlags.SupportsSaving, new string[] { ".texture" })
        {
        }

        protected override Document OnLoad(System.IO.Stream input)
        {
            BinaryReader reader = new BinaryReader(input, Encoding.Default);

            uint version       = reader.ReadUInt32();
            uint id            = reader.ReadUInt32();
            uint textureFormat = reader.ReadUInt32();
            uint imageWidth    = reader.ReadUInt32();
            uint imageHeight   = reader.ReadUInt32();
            uint numFrames     = reader.ReadUInt32();
            uint imageSize     = reader.ReadUInt32();

            byte[] imageData = reader.ReadBytes((int)imageSize);

            // Image data is color swapped, so swap it back to render in Paint.net properly
            uint p = 0;
            do
            {
                byte channel1 = imageData[p];
                byte channel3 = imageData[p+2];

                imageData[p] = channel3;
                imageData[p + 2] = channel1;

                p += 4;
            } while (p < imageSize); 

            Bitmap bitmap = new Bitmap((int)imageWidth, (int)imageHeight, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            var BoundsRect = new Rectangle(0, 0, (int)imageWidth, (int)imageHeight);
            BitmapData bmpData = bitmap.LockBits(BoundsRect, ImageLockMode.WriteOnly, bitmap.PixelFormat);
            IntPtr ptr = bmpData.Scan0;
            int bytes = bmpData.Stride * bitmap.Height;
            Marshal.Copy(imageData, 0, ptr, bytes);
            bitmap.UnlockBits(bmpData);
            
            return Document.FromImage(bitmap);
        }

        protected override void OnSave(Document input, System.IO.Stream output, SaveConfigToken token, Surface scratchSurface, ProgressEventHandler callback)
        {
            RenderArgs ra = new RenderArgs(new Surface(input.Size));
            input.Render(ra, true);

            UInt32 version = 1;
            UInt32 id = 0;
            UInt32 textureFormat = 0;
            UInt32 numFrames = 1;

            BinaryWriter writer = new BinaryWriter(output, Encoding.Default);
            writer.Write(version);
            writer.Write(id);
            writer.Write(textureFormat);
            writer.Write((UInt32)ra.Bitmap.Width);
            writer.Write((UInt32)ra.Bitmap.Height);
            writer.Write(numFrames);

            UInt32 imageSize = (UInt32)(ra.Bitmap.Width * ra.Bitmap.Height * 4);
            writer.Write(imageSize);

            var BoundsRect = new Rectangle(0, 0, (int)ra.Bitmap.Width, (int)ra.Bitmap.Height);
            BitmapData bmpData = ra.Bitmap.LockBits(BoundsRect, ImageLockMode.ReadOnly, ra.Bitmap.PixelFormat);
            IntPtr ptr = bmpData.Scan0;
            int bytes = bmpData.Stride * ra.Bitmap.Height;
            byte[] imageData = new byte[bytes];

            Marshal.Copy(ptr, imageData, 0, bytes);

            ra.Bitmap.UnlockBits(bmpData);

            // Image data is color swapped, so swap it back to save out
            uint p = 0;
            do
            {
                byte channel1 = imageData[p];
                byte channel3 = imageData[p + 2];

                imageData[p] = channel3;
                imageData[p + 2] = channel1;

                p += 4;
            } while (p < imageSize);

            writer.Write(imageData);
            writer.Write((float)0.0f); // left
            writer.Write((float)0.0f); // top
            writer.Write((float)ra.Bitmap.Width); // width
            writer.Write((float)ra.Bitmap.Height); // height
        }
    }
}
