using System;
using System.Globalization;

namespace RetroSpy.Readers
{
    public static class MiSTerReader
    {
        private static readonly string[] AXES_NAMES = {
            "x", "y", "z", "rx", "ry", "rz", "s0", "s1"
        };


        private static int ReadPacketChars(byte[] packet, ref int offset, int length)
        {
			int ret = Convert.ToInt32(System.Text.Encoding.Default.GetString(packet, offset, length), 16);
			offset += length;
            return ret;
        }
        public static ControllerStateEventArgs? ReadFromPacket(byte[]? packet)
        {
            if (packet == null)
            {
                throw new ArgumentNullException(nameof(packet));
            }

            if (packet.Length < 4)
            {
                return null;
            }

            int offset = 0;

            // If it ain't prefixed with RS then it's junk and can be ignored.
			if(System.Text.Encoding.Default.GetString(packet, offset, 2) != "RS")
            {
                return null;
            }
            offset += 2;

            // If the character S or M follows then the packet is keyboard-related.
            if(System.Text.Encoding.Default.GetString(packet, offset, 1) == "S" ||
				System.Text.Encoding.Default.GetString(packet, offset, 1) == "M")
            {
                // TODO: HANDLE.
                return null;
            }

			// It's no Base64, but the packets coming in are encoded as hex strings w/ no 0x,
			// which is 4x more efficient than just '0' or '1', giving 512B instead of 128B to work with.
			int axes = ReadPacketChars(packet, ref offset, 2);
			int buttons = ReadPacketChars(packet, ref offset, 2);

			int packetSize = 2 + 4 + (axes * 8) + ((buttons + 3) / 4) + 1;

            if (packet.Length != packetSize)
            {
                return null;
            }

            byte[] buttonValues = new byte[buttons];
            int[] axesValues = new int[axes];

            for (int i = 0; i < buttons;)
            {
				int accum = ReadPacketChars(packet, ref offset, 1);
                for (byte j = 0; j < 4 && i < buttons; ++j)
                {
                    buttonValues[i] = (byte)((accum >> (i % 4)) & 1);
                    i++;
                }
			}


			for (int i = 0; i < axes; ++i)
            {
                axesValues[i] = ReadPacketChars(packet, ref offset, 8);
			}

            ControllerStateBuilder outState = new();

            for (int i = 0; i < buttonValues.Length; ++i)
            {
                outState.SetButton("b" + i.ToString(CultureInfo.CurrentCulture), buttonValues[i] != 0x00);
            }

            for (int i = 0; i < axesValues.Length; ++i)
            {
                if (i < AXES_NAMES.Length)
                {
                    outState.SetAnalog(AXES_NAMES[i], axesValues[i] / (float)short.MaxValue, axesValues[i]);
                }

                outState.SetAnalog("a" + i.ToString(CultureInfo.CurrentCulture), axesValues[i] / (float)short.MaxValue, axesValues[i]);
            }

            if (axes >= 2)
            {
                if (axesValues[axes - 2] < 0)
                {
                    outState.SetButton("left", true);
                    outState.SetButton("right", false);
                }
                else if (axesValues[axes - 2] > 0)
                {
                    outState.SetButton("right", true);
                    outState.SetButton("left", false);
                }
                else
                {
                    outState.SetButton("left", false);
                    outState.SetButton("right", false);
                }

                if (axesValues[axes - 1] < 0)
                {
                    outState.SetButton("up", true);
                    outState.SetButton("down", false);
                }
                else if (axesValues[axes - 1] > 0)
                {
                    outState.SetButton("down", true);
                    outState.SetButton("up", false);
                }
                else
                {
                    outState.SetButton("up", false);
                    outState.SetButton("down", false);
                }
            }
            else
            {
                outState.SetButton("up", false);
                outState.SetButton("down", false);
                outState.SetButton("left", false);
                outState.SetButton("right", false);
            }

            return outState.Build();
        }
    }
}