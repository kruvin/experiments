/*
		2011 Takahiro Harada
*/

template<>
class BoundSearch<TYPE_HOST> : public BoundSearchBase
{
	public:
		typedef Launcher::BufferInfo BufferInfo;

		struct Data
		{
			const Device* m_device;
		};

		static
		Data* allocate(const Device* deviceData, int maxSize = 0)
		{
			ADLASSERT( deviceData->m_type == TYPE_HOST );
			Data* data = new Data;
			data->m_device = deviceData;
			return data;
		}

		static
		void deallocate(Data* data)
		{
			delete data;
		}

		static
		void execute(Data* data, Buffer<SortData>& rawSrc, u32 nSrc, Buffer<u32>& rawDst, u32 nDst, Option option = BOUND_LOWER)
		{
			ADLASSERT( rawSrc.getType() == TYPE_HOST );
			ADLASSERT( rawDst.getType() == TYPE_HOST );

			HostBuffer<SortData>& src = *(HostBuffer<SortData>*)&rawSrc;
			HostBuffer<u32>& dst = *(HostBuffer<u32>*)&rawDst;

			for(int i=0; i<nSrc-1; i++) 
				ADLASSERT( src[i].m_key <= src[i+1].m_key );

			if( option == BOUND_LOWER )
			{
				for(u32 i=0; i<nSrc; i++)
				{
					SortData& iData = (i==0)? SortData(-1,-1): src[i-1];
					SortData& jData = (i==nSrc)? SortData(nDst, nDst): src[i];

					if( iData.m_key != jData.m_key )
					{
//						for(u32 k=iData.m_key+1; k<=min(jData.m_key,nDst-1); k++)
						u32 k = jData.m_key;
						{
							dst[k] = i;
						}
					}
				}
			}
			else if( option == BOUND_UPPER )
			{
				for(u32 i=0; i<nSrc+1; i++)
				{
					SortData& iData = (i==0)? SortData(0,0): src[i-1];
					SortData& jData = (i==nSrc)? SortData(nDst, nDst): src[i];

					if( iData.m_key != jData.m_key )
					{
//						for(u32 k=iData.m_key; k<min(jData.m_key,nDst); k++)
						u32 k = iData.m_key;
						{
							dst[k] = i;
						}
					}
				}
			}
			else if( option == COUNT )
			{
				HostBuffer<u32> lower( data->m_device, nDst );
				HostBuffer<u32> upper( data->m_device, nDst );

				for(u32 i=0; i<nDst; i++) { lower[i] = upper[i] = 0; }

				execute( data, rawSrc, nSrc, lower, nDst, BOUND_LOWER );
				execute( data, rawSrc, nSrc, upper, nDst, BOUND_UPPER );

				for(u32 i=0; i<nDst; i++) { dst[i] = upper[i] - lower[i]; }
			}
			else
			{
				ADLASSERT( 0 );
			}
		}

//		static
//		void execute(Data* data, Buffer<u32>& src, Buffer<u32>& dst, int n, Option option = );
};


