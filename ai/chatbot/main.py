import os
from dotenv import load_dotenv
from langchain_groq import ChatGroq
from langchain_huggingface import HuggingFaceEmbeddings
from langchain_community.vectorstores import InMemoryVectorStore
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_community.document_loaders import TextLoader
from langchain_core.prompts import PromptTemplate
from langchain_core.output_parsers import StrOutputParser
from langchain_core.runnables import RunnablePassthrough, RunnableLambda

# Prevent tokenizer warnings/errors
os.environ["TOKENIZERS_PARALLELISM"] = "false"

# Load environment variables
load_dotenv()
GROQ_API_KEY = os.getenv("GROQ_API_KEY")

# Initialize the language model
llm = ChatGroq(
    model_name="llama-3.3-70b-versatile",
    temperature=0.5,
    api_key=GROQ_API_KEY
)

# Initialize the embeddings model
embeddings_model = HuggingFaceEmbeddings(model_name="intfloat/multilingual-e5-large-instruct")

# Setup text splitter and document loader
text_splitter = RecursiveCharacterTextSplitter(chunk_size=500, chunk_overlap=100)
loader = TextLoader("yoda.txt", encoding="utf-8")

# Load and split the document into chunks
document = loader.load()
chunks = text_splitter.split_documents(document)

# Create an in-memory vector store from the document chunks
vector_store = InMemoryVectorStore.from_documents(chunks, embeddings_model)
retriever = vector_store.as_retriever()

# Define the prompt template
template = """
You are a helpful assistant that can answer questions about Yoda's restaurant.
Answer in the tone of Yoda.

Question: {question}

Only use the provided context to answer the question.
Context: {context}
"""
prompt = PromptTemplate.from_template(template)

# Create a runnable lambda that extracts the "question" from input and passes it to the retriever
question_to_context = RunnableLambda(lambda inp: retriever.get_relevant_documents(inp["question"]))

# Build the chain ensuring that the retriever only receives the question string
chain = {
    "context": question_to_context,
    "question": RunnablePassthrough()
} | prompt | llm | StrOutputParser()

# Invoke the chain with the question input
print(chain.invoke({"question": "Have Lando Calrissian been there? What is the open hours?"}))
