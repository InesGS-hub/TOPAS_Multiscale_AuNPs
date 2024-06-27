import pandas as pd
import os
import numpy as np

def ler_cabecalho(nome_arquivo_header):
    with open(nome_arquivo_header, 'r') as f:
        linhas = f.readlines()
    
    num_historias_originais = num_historias_reached = num_scored_particles = 0

    for linha in linhas:
        if "Number of Original Histories:" in linha:
            num_historias_originais = int(linha.split(":")[1].strip())
        if "Number of Original Histories that Reached Phase Space:" in linha:
            num_historias_reached = int(linha.split(":")[1].strip())
        if "Number of Scored Particles:" in linha:
            num_scored_particles = int(linha.split(":")[1].strip())
    
    return num_historias_originais, num_historias_reached, num_scored_particles, linhas

def ler_phsp_binario(caminho_arquivo_phsp, dtype):
    if is_binary_file(caminho_arquivo_phsp):
        dados = np.fromfile(caminho_arquivo_phsp, dtype=dtype)
        df = pd.DataFrame(dados)
        return df, True
    else:
        return "Formato ASCII detectado.", False

def is_binary_file(file_path, num_bytes=1024):
    with open(file_path, 'rb') as f:
        data = f.read(num_bytes)
    text_characters = bytearray({7, 8, 9, 10, 12, 13, 27} | set(range(0x20, 0x7F)))
    return bool(data.translate(None, text_characters))

def dividir_phsp(diretorio_arquivo, nome_arquivo, dtype, num_historias_por_arquivo=10000):
    caminho_arquivo_phsp = os.path.join(diretorio_arquivo, nome_arquivo + ".phsp")
    caminho_arquivo_header = os.path.join(diretorio_arquivo, nome_arquivo + ".header")
    
    num_historias_originais, num_historias_reached, num_scored_particles, cabecalho = ler_cabecalho(caminho_arquivo_header)
    
    df, is_binary = ler_phsp_binario(caminho_arquivo_phsp, dtype)
    
    if not is_binary:
        print(df)  # Mensagem de formato ASCII
        return
    
    ids_historia_unicos = df['EventID'].unique()
    num_arquivos = (len(ids_historia_unicos) // num_historias_por_arquivo) + int(len(ids_historia_unicos) % num_historias_por_arquivo != 0)
    
    dir_saida = os.path.join(diretorio_arquivo, f"saida_{nome_arquivo}")
    os.makedirs(dir_saida, exist_ok=True)
    
    for i in range(num_arquivos):
        ids_historia_chunk = ids_historia_unicos[i*num_historias_por_arquivo:(i+1)*num_historias_por_arquivo]
        chunk_df = df[df['EventID'].isin(ids_historia_chunk)]
        
        chunk_path = os.path.join(dir_saida, f"{nome_arquivo}_{i+1}.phsp")
        chunk_df.to_records(index=False).tofile(chunk_path)
        
        num_electrons = (chunk_df['ParticleType'] == 11).sum()
        num_protons = (chunk_df['ParticleType'] == -11).sum()
        num_photons = (chunk_df['ParticleType'] == 22).sum()

        min_ke_electrons = chunk_df.loc[chunk_df['ParticleType'] == 11, 'InitialKineticEnergy'].min()
        min_ke_protons = chunk_df.loc[chunk_df['ParticleType'] == -11, 'InitialKineticEnergy'].min()
        min_ke_photons = chunk_df.loc[chunk_df['ParticleType'] == 22, 'InitialKineticEnergy'].min()

        max_ke_electrons = chunk_df.loc[chunk_df['ParticleType'] == 11, 'InitialKineticEnergy'].max()
        max_ke_protons = chunk_df.loc[chunk_df['ParticleType'] == -11, 'InitialKineticEnergy'].max()
        max_ke_photons = chunk_df.loc[chunk_df['ParticleType'] == 22, 'InitialKineticEnergy'].max()
        
        header_path = os.path.join(dir_saida, f"{nome_arquivo}_{i+1}.header")
        with open(header_path, 'w') as f:
            for linha in cabecalho:
                if "Number of Original Histories that Reached Phase Space:" in linha:
                    f.write(f"Number of Original Histories that Reached Phase Space: {num_historias_por_arquivo}\n")
                elif "Number of Scored Particles:" in linha:
                    f.write(f"Number of Scored Particles: {len(chunk_df)}\n")
                elif "Number of e-:" in linha:
                    f.write(f"Number of e-: {num_electrons}\n")
                elif "Number of e+:" in linha:
                    f.write(f"Number of e+: {num_protons}\n")
                elif "Number of gamma:" in linha:
                    f.write(f"Number of gamma: {num_photons}\n")

                elif "Minimum Kinetic Energy of e-:" in linha:
                    f.write(f"Minimum Kinetic Energy of e-: {min_ke_electrons:.8f} MeV\n")
                elif "Minimum Kinetic Energy of e+:" in linha:
                    f.write(f"Minimum Kinetic Energy of e+: {min_ke_protons:.8f} MeV\n")
                elif "Minimum Kinetic Energy of gamma:" in linha:
                    f.write(f"Minimum Kinetic Energy of gamma: {min_ke_photons:.8f} MeV\n")
                    
                elif "Maximum Kinetic Energy of e-:" in linha:
                    f.write(f"Maximum Kinetic Energy of e-: {max_ke_electrons:.8f} MeV\n")
                elif "Maximum Kinetic Energy of e+:" in linha:
                    f.write(f"Maximum Kinetic Energy of e+: {max_ke_protons:.8f} MeV\n")
                elif "Maximum Kinetic Energy of gamma:" in linha:
                    f.write(f"Maximum Kinetic Energy of gamma: {max_ke_photons:.8f} MeV\n")
                else:
                    f.write(linha)

    print(f"Foram criados {num_arquivos} arquivos em: {dir_saida}")

dtype = np.dtype([
    ('PositionX', 'f4'),
    ('PositionY', 'f4'),
    ('PositionZ', 'f4'),
    ('DirectionCosineX', 'f4'),
    ('DirectionCosineY', 'f4'),
    ('Energy', 'f4'),
    ('Weight', 'f4'),
    ('ParticleType', 'i4'),
    ('ThirdDirectionCosineNegativeFlag', 'b1'),
    ('FirstScoredParticleFlag', 'b1'),
    ('TOPASTime', 'f4'),
    ('TimeOfFlight', 'f4'),
    ('RunID', 'i4'),
    ('EventID', 'i4'),
    ('TrackID', 'i4'),
    ('ParentID', 'i4'),
    ('InitialKineticEnergy', 'f4'),
    ('VertexPositionX', 'f4'),
    ('VertexPositionY', 'f4'),
    ('VertexPositionZ', 'f4'),
    ('InitialDirectionCosineX', 'f4'),
    ('InitialDirectionCosineY', 'f4'),
    ('InitialDirectionCosineZ', 'f4'),
    ('SeedPart1', 'i4'),
    ('SeedPart2', 'i4'),
    ('SeedPart3', 'i4'),
    ('SeedPart4', 'i4')
])

# Chamar a função com o nome do arquivo PHSP (sem extensão)
#dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_0_0", dtype)
dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_0_6mil", dtype)
#dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_0_12mil", dtype)
#dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_0_120mil", dtype)
#dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_120mil_0", dtype)
#dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_108mil_12mil", dtype)
#dividir_phsp("/media/mcbeam/HDD/Documents/Ines", "PHSP_Nuc_114mil_6mil", dtype)"""







